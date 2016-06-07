#include "kernelSetup.h"

#define DEBUG 0
#define PROFILE 0

int createProgram(int l_selTask, char* srcPath,int numTasks){
	int status;
	FILE *fp;

		fp = fopen(srcPath, "r");
		if (!fp) {
			fprintf(stderr, "Can not find or load kernels source file.\n");
			perror ("The following error occurred");
			exit(1);
		}

		fseek(fp, 0L, SEEK_END);
		int sz = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		l_taskList[l_selTask].code = malloc(sizeof(XCLcode));
		l_taskList[l_selTask].code[0].source_str = (char*) malloc(sz+1);
		l_taskList[l_selTask].code[0].source_size = fread(l_taskList[l_selTask].code[0].source_str,
				1, sz, fp);

		l_taskList[l_selTask].CLprogram = malloc(sizeof(cl_program));

		l_taskList[l_selTask].CLprogram[0] = clCreateProgramWithSource(
				l_taskList[l_selTask].device[0].context, 1,
				(const char **) &l_taskList[l_selTask].code[0].source_str,
				(const size_t *) &l_taskList[l_selTask].code[0].source_size, &status);

		chkerr(status, "Creating Program ", __FILE__, __LINE__);
	fclose(fp);

	return status;
}

int buildProgram(int l_selTask, int numTasks) {
	int status;

		status = clBuildProgram(l_taskList[l_selTask].CLprogram[0], 1,
				&l_taskList[l_selTask].device[0].deviceId, NULL, NULL, NULL);

		if (status == CL_BUILD_PROGRAM_FAILURE) {
			// Determine the size of the log
			size_t log_size;
			clGetProgramBuildInfo(l_taskList[l_selTask].CLprogram[0], l_taskList[l_selTask].device[0].deviceId, CL_PROGRAM_BUILD_LOG,
					0, NULL, &log_size);

			// Allocate memory for the log
			char *log = (char *) malloc(log_size+1);
			// Get the log
			clGetProgramBuildInfo(l_taskList[l_selTask].CLprogram[0], l_taskList[l_selTask].device[0].deviceId, CL_PROGRAM_BUILD_LOG,
					log_size, log, NULL);
			log[log_size+1] = '\0';
			// Print the log
			printf("log Description: %s\n", log);
		}

		chkerr(status, "Building Program ", __FILE__, __LINE__);

	return status;
}

int createKernel(int l_selTask, char* kernelName, int numTasks){  //TODO: here we will need a list of kernels.
	int status;
	//int i;
	//for(i=0;i<numTasks;i++){
	l_taskList[l_selTask].kernel=malloc(sizeof(XCLkernel));
	l_taskList[l_selTask].kernel[0].kernel = clCreateKernel(l_taskList[l_selTask].CLprogram[0], kernelName, &status);
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

	status=clGetKernelInfo(l_taskList[l_selTask].kernel[0].kernel,CL_KERNEL_FUNCTION_NAME,0,NULL,&kernNameSize);
	char* kernName=malloc(kernNameSize*sizeof(char));
	status=clGetKernelInfo(l_taskList[l_selTask].kernel[0].kernel,CL_KERNEL_FUNCTION_NAME,kernNameSize,(char*)kernName,NULL);

	status=clGetKernelInfo(l_taskList[l_selTask].kernel[0].kernel,CL_KERNEL_NUM_ARGS,sizeof(cl_uint),&numArgs,NULL);

	chkerr(status, "error at: getting kernel info", __FILE__, __LINE__);

	debug_print("\n Debug:  %s, has %u args\n",kernName,numArgs);

	l_taskList[l_selTask].kernel[0].numArgs=numArgs;
	//}

return status;
}

int setKernelmemObj(int seltask,int numparameter,int paramSize,int trayIdx){
	int status;

	//for (i = 0; i < numTasks; i++) {
		int myRack=l_taskList[seltask].Rack;
	debug_print("\n Debug: setting cl_mem arg: %d in task %d from rack: [%d] tray:[%d] \n", numparameter, seltask,myRack,trayIdx);
		status = clSetKernelArg(l_taskList[seltask].kernel[0].kernel, numparameter,
				sizeof(cl_mem), &l_taskList[seltask].device[0].memHandler[myRack][trayIdx]);
	chkerr(status, "error at: Setting entity buffer Arg.", __FILE__, __LINE__);
	//}
	return 0;
}

int setKernelArgs(int seltask,int numparameter,int paramSize,void* paramValue){
	int status;
	int i;

	//for (i = 0; i < numTasks; i++) {
		debug_print("\n Debug: setting arg: %d in task %d \n", numparameter, seltask);

		status = clSetKernelArg(l_taskList[seltask].kernel[0].kernel, numparameter,
				paramSize, (void *) paramValue);


		chkerr(status, "error at: Setting other kernel Args", __FILE__,	__LINE__);
	//}

	debug_print("set Args %d successful..\n",numparameter);
	return status;
}




void * enqueTaskReq(void *args) {
	int status=0;
	int i; //indx var.
	enqueueArgs_t* l_args = (enqueueArgs_t*) args;

	char* profileFlag;
		int profilingEnabled = 0;
		profileFlag = getenv("XSCALA_PROFILING_APP");
		if (profileFlag != NULL) {
			profilingEnabled = 1;
		}

	if (profilingEnabled) {

		//****************************
		struct timeval tval_ini, tval_fini, tval_IniResult, tval_FiniResult,tval_total; //these structures will provide the ini--fini times for profiling.
		gettimeofday(&tval_ini, NULL);

		/*printf("\n global_Initime: %ld.%06ld\n", (long int) tval_globalInit.tv_sec,
								(long int) tval_globalInit.tv_usec);
		printf("\n initime: %ld.%06ld\n", (long int) tval_ini.tv_sec,
										(long int) tval_ini.tv_usec);*/

		timersub(&tval_ini, &tval_globalInit, &tval_IniResult);
		double secsIni = (double) tval_IniResult.tv_sec;
		double milsIni = (double) (tval_IniResult.tv_usec) / 1000000;



		//*****************************


		cl_ulong time_start, time_end;
			double total_time;
		cl_event kernelProfileEvent;
		status = clEnqueueNDRangeKernel(l_args->th_queue, l_args->th_kernel,
				l_args->workDim, NULL, l_args->globalThreads,
				l_args->localThreads, 0, NULL, &kernelProfileEvent);

		//chkerr(status,"enqueuing the kernel:", __FILE__, __LINE__);



		clFlush(l_args->th_queue);
		clFinish(l_args->th_queue);

		clWaitForEvents(1, &kernelProfileEvent);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(kernelProfileEvent, CL_PROFILING_COMMAND_END,
				sizeof(time_end), &time_end, NULL);
		//***********************************************

		gettimeofday(&tval_fini, NULL );
		timersub(&tval_fini, &tval_globalInit, &tval_FiniResult);
		double secsFini = (double) tval_FiniResult.tv_sec;
		double milsFini = (double) (tval_FiniResult.tv_usec) / 1000000;



		int myRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
		FILE* schedImg;
		schedImg = fopen("./schedImg.txt", "a");

		for(i=0;i<g_numTasks;i++){
			if(g_taskList[i].r_rank==myRank && g_taskList[i].l_taskIdx==l_args->g_selTsk ){
				fprintf(schedImg, " %ld.%06ld ",(long int) tval_IniResult.tv_sec,
						(long int) tval_IniResult.tv_usec);
				//printf(" %ld.%06ld ", (long int) tval_IniResult.tv_sec,(long int) tval_IniResult.tv_usec);
				fprintf(schedImg," %ld.%06ld ", (long int) tval_FiniResult.tv_sec,
										(long int) tval_FiniResult.tv_usec);
				fprintf(schedImg, " %d \n",g_taskList[i].g_taskIdx);


				timersub(&tval_fini, &tval_ini, &tval_total);
				double secs = (double) tval_total.tv_sec;
				double mils = (double) (tval_total.tv_usec) / 1000000;
				if(status!=0){
					printf("Exec status of task %d : %d \n",g_taskList[i].g_taskIdx,status);
				}
				else{
				printf("Execution time of task %d: -->  %ld.%06ld secs. \n",g_taskList[i].g_taskIdx,(long int) tval_total.tv_sec,
						(long int) tval_total.tv_usec);
				}
				break;
			}
		}
		fclose(schedImg);
		//*********************************************


		total_time = time_end - time_start;
		//printf("Execution time of task %d: -->  %0.3f ms \n",l_args->g_selTsk,(total_time / 1000000.0));

	}
	else{
		//TODO: Only one thread must enqueue at time (concurrency).
		//MUST USE multiple mutex to enqueue procedures involves different queues (devices).

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

	status = clEnqueueNDRangeKernel(l_taskList[i].device[0].queue, l_taskList[i].kernel[0].kernel, 1, NULL,
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

}else{ //sel task != -1

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	//cl_event kernelProfileEvent;

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

		/*--if(thds==NULL)
			thds=(pthread_t*)malloc(numTasks*sizeof(pthread_t));

		if(th_Args==NULL)
			th_Args=(enqueueArgs_t **)malloc(numTasks*sizeof(enqueueArgs_t*)); --*/

		/*pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);*/

/*--		int ret;
		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);--*/


		/*--th_Args[selTask]=(enqueueArgs_t*)malloc(sizeof(enqueueArgs_t));
		th_Args[selTask]->globalThreads=globalThreads;
		th_Args[selTask]->localThreads=localThreads;
		th_Args[selTask]->workDim=workDim;
		//th_Args[selTask]->kernelProfileEvent=kernelProfileEvent;
		th_Args[selTask]->th_queue=l_taskList[selTask].device[0].queue;
		th_Args[selTask]->th_kernel=l_taskList[selTask].kernel[0].kernel;
		th_Args[selTask]->g_selTsk=selTask; //TODO this is not the global, in fact is the local =( --*/

		enqueueArgs_t* th_Args= (enqueueArgs_t*)malloc(sizeof(enqueueArgs_t));

		th_Args->globalThreads=globalThreads;
		th_Args->localThreads=localThreads;
		th_Args->workDim=workDim;
		//th_Args->kernelProfileEvent=kernelProfileEvent;
		th_Args->th_queue=l_taskList[selTask].device[0].queue;
		th_Args->th_kernel=l_taskList[selTask].kernel[0].kernel;
		th_Args->g_selTsk=selTask;

		enqueTaskReq((void*)th_Args);
		//pthread_create(&thds[selTask], NULL, enqueTaskReq, (void*) th_Args[selTask]);
	  	debug_print(" -- task %d requested in rank: %d -- \n",selTask,myRank);



	  	//printf("task: %d rank  Requested at time: %s \n",(numReq++),asctime(timeinfo));
		//pthread_attr_destroy(&attr);

/*
		status = clEnqueueNDRangeKernel(l_taskList[selTask].device[0].queue, l_taskList[selTask].kernel[0].kernel, 1, NULL,
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






