#include "taskProcedures.h"

#define DEBUG 0
#define PROFILE 0

int createProgram(int l_selTask, char* srcPath,int numTasks){
	int status;
	//int i;
	FILE *fp;
	//for (i = 0; i < numTasks; i++) {
		fp = fopen(srcPath, "r");
		if (!fp) {
			fprintf(stderr, "Can not find or load kernels source file.\n");
			perror ("The following error occurred");
			exit(1);
		}

		fseek(fp, 0L, SEEK_END);
		int sz = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		taskList[l_selTask].code = malloc(sizeof(XCLcode));
		taskList[l_selTask].code[0].source_str = (char*) malloc(sz+1);
		taskList[l_selTask].code[0].source_size = fread(taskList[l_selTask].code[0].source_str,
				1, sz, fp);

		taskList[l_selTask].CLprogram = malloc(sizeof(cl_program));

		taskList[l_selTask].CLprogram[0] = clCreateProgramWithSource(
				taskList[l_selTask].device[0].context, 1,
				(const char **) &taskList[l_selTask].code[0].source_str,
				(const size_t *) &taskList[l_selTask].code[0].source_size, &status);

		chkerr(status, "Creating Program ", __FILE__, __LINE__);
	fclose(fp);
	//}

	return status;
}

int buildProgram(int l_selTask, int numTasks) {
	int status;
	//int i;
	//for (i = 0; i < numTasks; i++) {
		status = clBuildProgram(taskList[l_selTask].CLprogram[0], 1,
				&taskList[l_selTask].device[0].deviceId, NULL, NULL, NULL);

		if (status == CL_BUILD_PROGRAM_FAILURE) {
			// Determine the size of the log
			size_t log_size;
			clGetProgramBuildInfo(taskList[l_selTask].CLprogram[0], taskList[l_selTask].device[0].deviceId, CL_PROGRAM_BUILD_LOG,
					0, NULL, &log_size);

			// Allocate memory for the log
			char *log = (char *) malloc(log_size+1);
			// Get the log
			clGetProgramBuildInfo(taskList[l_selTask].CLprogram[0], taskList[l_selTask].device[0].deviceId, CL_PROGRAM_BUILD_LOG,
					log_size, log, NULL);
			log[log_size+1] = '\0';
			// Print the log
			printf("log Description: %s\n", log);
		}

		chkerr(status, "Building Program ", __FILE__, __LINE__);
	//}

	return status;
}

int createKernel(int l_selTask, char* kernelName, int numTasks){  //TODO: here we will need a list of kernels.
	int status;
	//int i;
	//for(i=0;i<numTasks;i++){
	taskList[l_selTask].kernel=malloc(sizeof(XCLkernel));
	taskList[l_selTask].kernel[0].kernel = clCreateKernel(taskList[l_selTask].CLprogram[0], kernelName, &status);
	chkerr(status, "error at: creating the kernel:", __FILE__, __LINE__);
	//}
	return status;
}


int kernelXplor(int l_selTask, int numTasks){
	int status;
	//int i;
	size_t kernNameSize;
	cl_uint numArgs;
	//for(i=0;i<numTasks;i++){

	status=clGetKernelInfo(taskList[l_selTask].kernel[0].kernel,CL_KERNEL_FUNCTION_NAME,0,NULL,&kernNameSize);
	char* kernName=malloc(kernNameSize*sizeof(char));
	status=clGetKernelInfo(taskList[l_selTask].kernel[0].kernel,CL_KERNEL_FUNCTION_NAME,kernNameSize,(char*)kernName,NULL);

	status=clGetKernelInfo(taskList[l_selTask].kernel[0].kernel,CL_KERNEL_NUM_ARGS,sizeof(cl_uint),&numArgs,NULL);

	chkerr(status, "error at: getting kernel info", __FILE__, __LINE__);

	debug_print("\n Debug:  %s, has %u args\n",kernName,numArgs);

	taskList[l_selTask].kernel[0].numArgs=numArgs;
	//}

return status;
}

int setKernelmemObj(int seltask,int numparameter,int paramSize,int trayIdx){
	int status;

	//for (i = 0; i < numTasks; i++) {
		int myRack=taskList[seltask].Rack;
	debug_print("\n Debug: setting cl_mem arg: %d in task %d from rack: [%d] tray:[%d] \n", numparameter, seltask,myRack,trayIdx);

		status = clSetKernelArg(taskList[seltask].kernel[0].kernel, numparameter,
				sizeof(cl_mem), &taskList[seltask].device[0].memHandler[myRack][trayIdx]);
	chkerr(status, "error at: Setting entity buffer Arg.", __FILE__, __LINE__);
	//}
	return 0;
}

int setKernelArgs(int seltask,int numparameter,int paramSize,void* paramValue){
	int status;
	int i;

	//for (i = 0; i < numTasks; i++) {
		debug_print("\n Debug: setting arg: %d in task %d \n", numparameter, seltask);
		status = clSetKernelArg(taskList[seltask].kernel[0].kernel, numparameter,
				paramSize, (void *) paramValue);
		chkerr(status, "error at: Setting other kernel Args", __FILE__,	__LINE__);
	//}

	debug_print("set Args %d successful..\n",numparameter);
	return status;
}


typedef struct enqueueArgs_st{
	size_t* globalThreads;
	size_t* localThreads;
	cl_command_queue th_queue;
	cl_kernel th_kernel;
	int workDim;
	int g_selTsk;
	//cl_event kernelProfileEvent;

}enqueueArgs_t;
enqueueArgs_t **th_Args;
pthread_t*  thds;

void * enqueTaskReq(void *args) {
	int status;
	enqueueArgs_t* l_args = (enqueueArgs_t*) args;

	char* profileFlag;
		int profilingEnabled = 0;
		profileFlag = getenv("XSCALA_PROFILING_APP");
		if (profileFlag != NULL) {
			profilingEnabled = 1;
		}

	if (profilingEnabled) {
		cl_ulong time_start, time_end;
			double total_time;
		cl_event kernelProfileEvent;
		status = clEnqueueNDRangeKernel(l_args->th_queue, l_args->th_kernel,
				l_args->workDim, NULL, l_args->globalThreads,
				l_args->localThreads, 0, NULL, &kernelProfileEvent);
		clFlush(l_args->th_queue);
		clFinish(l_args->th_queue);

		clWaitForEvents(1, &kernelProfileEvent);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_END,
				sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		printf("Execution time of task %d: -->  %0.3f ms \n",l_args->g_selTsk,(total_time / 1000000.0));

	}
		else{
	status = clEnqueueNDRangeKernel(l_args->th_queue, l_args->th_kernel, l_args->workDim, NULL,
			l_args->globalThreads, l_args->localThreads, 0, NULL,
			NULL);
	clFlush(l_args->th_queue);
	clFinish(l_args->th_queue);
		}
	//chkerr(status, "Enqueuing Kernels ", __FILE__, __LINE__);
	//debug_print("Enqueuing Kernel successful..\n");
	debug_print("Enqueue Kernel successful..\n");
	//pthread_exit(NULL);
}

//int enqueueKernel(int numTasks,int selTask, int workDim, const size_t globalThreads, const size_t localThreads) {
int enqueueKernel(int numTasks,int selTask, int workDim, const size_t* globalThreads, const size_t* localThreads) {
	int status;
	int i;

if(selTask==-1){ // -1 means all tasks must enqueue this kernel.

	for(i=0;i<numTasks;i++){
	cl_event kernelProfileEvent;
	cl_ulong time_start, time_end;
	double total_time;

	status = clEnqueueNDRangeKernel(taskList[i].device[0].queue, taskList[i].kernel[0].kernel, 1, NULL,
			&globalThreads, &localThreads, 0, NULL, &kernelProfileEvent);
	chkerr(status, "Enqueuing Kernels ", __FILE__, __LINE__);

	debug_print("Enqueuing Kernel successful..\n");


	#if PROFILE
	clWaitForEvents(1 , &kernelProfileEvent);
	clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;
	profile_print("Execution time of task: %d -->  %0.3f ms\n",i,(total_time / 1000000.0));
	#endif //if PROFILE
	}

}else{

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


	cl_event kernelProfileEvent;
#if PROFILE
	cl_ulong time_start, time_end;
	double total_time;

		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		printf("scheduled HOUR of task: %d -->  %s \n", selTask,
				asctime(timeinfo));
#endif //if PROFILE

		/*
		 * This must be done with pthreads because clEnqueueNDRangeKernel is synchronous for
		 * some openCL implementations like NVIDIA ='(...
		 *
		 * */

		if(thds==NULL)
			thds=(pthread_t*)malloc(numTasks*sizeof(pthread_t));

		if(th_Args==NULL)
			th_Args=(enqueueArgs_t **)malloc(numTasks*sizeof(enqueueArgs_t*));

		/*pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);*/

		int ret;
		static int numReq=0;

		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);



		th_Args[selTask]=(enqueueArgs_t*)malloc(sizeof(enqueueArgs_t));
		th_Args[selTask]->globalThreads=globalThreads;
		th_Args[selTask]->localThreads=localThreads;
		th_Args[selTask]->workDim=workDim;
		//th_Args[selTask]->kernelProfileEvent=kernelProfileEvent;
		th_Args[selTask]->th_queue=taskList[selTask].device[0].queue;
		th_Args[selTask]->th_kernel=taskList[selTask].kernel[0].kernel;
		th_Args[selTask]->g_selTsk=selTask;


		//printf("task: %d Requesting at time: %s \n",numReq++,asctime(timeinfo));
	  	pthread_create(&thds[selTask], NULL, enqueTaskReq, (void*) th_Args[selTask]);
	  	debug_print(" -- task %d requested in rank: %d -- \n",selTask,myRank);
	  	//printf("task: %d rank  Requested at time: %s \n",(numReq++),asctime(timeinfo));
		//pthread_attr_destroy(&attr);

/*
		status = clEnqueueNDRangeKernel(taskList[selTask].device[0].queue, taskList[selTask].kernel[0].kernel, 1, NULL,
				&globalThreads, &localThreads, 0, NULL, &kernelProfileEvent);
		chkerr(status, "Enqueuing Kernels ", __FILE__, __LINE__);
		  debug_print("Enqueuing Kernel requested..\n");*/

#if PROFILE
		clWaitForEvents(1 , &kernelProfileEvent);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		profile_print("Execution time of task: %d in rank %d -->  %0.3f ms \n",selTask, myRank,(total_time / 1000000.0));
#endif //if PROFILE

}
	return status;
}


int XclWaitAllTasks(int numTasks,MPI_Comm comm){
	int i;
	for(i=0;i<numTasks;i++){
				pthread_join(thds[i], NULL);
	}
	/*for(i=0;i<numTasks;i++){
		pthread_detach(thds[i]);
	}*/
	MPI_Barrier(comm);
	return 0;
}


int XclWaitFor(int l_numTasks, int* l_taskIds, MPI_Comm comm){
	int i,j;
	debug_print("waiting for %d \n", *l_taskIds);
		for(i=0;i<l_numTasks;i++){
			j=l_taskIds[i];
			pthread_join(thds[j], NULL);
		}
		//MPI_Barrier(comm);
		return 0;
}

