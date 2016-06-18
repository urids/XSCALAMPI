
#include "taskManager.h"


XCLtask* l_taskList; // Global Variable declared in task.h
int l_numTasks;//Global variable declared in task.h

void* waitSubroutine(void*Args){

	sem_t* sem=((struct Args_waitSubroutine_st*)Args)->semaphore;
	sem_wait(sem);
	return (void*)0;
}

void* signalSubroutine(void*Args){

	sem_t* sem=((struct Args_signalSubroutine_st*)Args)->semaphore;
	sem_post(sem);
	return (void*)0;
}

//pthread_mutex_t setProcedureMutex=PTHREAD_MUTEX_INITIALIZER; //this mutex are requiered to
//pthread_mutex_t execTaskMutex=PTHREAD_MUTEX_INITIALIZER;	//handle non thread safety in certain OCL API calls

//int _OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath,char* kernelName){

int _OMPI_XclSetProcedure(void* Args){
	//1.- unwrap Args.
	struct Args_SetProcedure_st * setProcedure_Args=(struct Args_SetProcedure_st *)Args;
	int l_selTask	=	setProcedure_Args->l_selTask;
	char* srcPath	=	setProcedure_Args->srcPath;
	char* kernelName=	setProcedure_Args->kernelName;

	//2.-The task thread makes the calls to the components
	void *dlhandle;
	int (*createProgram)(int l_selTask, char*,int);
	int (*buildProgram)(int l_selTask, int);
	int (*createKernel)(int l_selTask, char*,int);
	int (*kernelXplor)(int l_selTask, int);

	char *error;

	dlhandle =dlopen("libkernelMgmt.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	createProgram = dlsym(dlhandle, "createProgram");
	buildProgram = dlsym(dlhandle, "buildProgram");
	createKernel = dlsym(dlhandle, "createKernel");
	kernelXplor = dlsym(dlhandle, "kernelXplor");
	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	debug_print("0.-kernelSrcFile: %s\n",srcPath);

	int err; //TODO: l_numTasks must be deleted (the 0) from the calls: it is no longer useful =/.

		err=(*createProgram)(l_selTask, srcPath,0);
		err|=(*buildProgram)(l_selTask, 0);
		err|=(*createKernel)(l_selTask, kernelName, 0);
		err|=(*kernelXplor)(l_selTask, 0);


	dlclose(dlhandle);

	return 0;
}


//int _OMPI_XclExecTask(MPI_Comm comm, int g_selTask, int workDim, size_t * globalThreads,
	//	size_t * localThreads, const char * fmt, va_list argsList) {

int _OMPI_XclExecTask(void * Args){

	//1.- Unwrap the Args.
	struct Args_ExecTask_st * execTask_Args=(struct Args_ExecTask_st *)Args;
	MPI_Comm comm = execTask_Args->comm;
	int l_selTask =	execTask_Args->l_selTask;
	int workDim   =	execTask_Args->workDim;
	size_t* globalThreads =	execTask_Args->globalThreads;
	size_t * localThreads =	execTask_Args->localThreads;
	const char* fmt = execTask_Args->fmt;
	va_list argsList;
	va_copy(argsList, execTask_Args->argsList);
	va_end(argsList);
	//argsList = execTask_Args->argsList;



		//int l_selTask= g_taskList[g_selTask].l_taskIdx;

		//2.-The task thread makes the calls to the components
		void *dlhandle;

		int (*argsParser)(MPI_Comm, int selTask, int workDim, size_t*, size_t*, const char *,
				va_list);
		int (*enqueueKernel)(int numTasks,int selTask, int workDim, const size_t* globalThreads, const size_t* localThreads);
		char *error;

		dlhandle = dlopen("libkernelMgmt.so", RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		argsParser = dlsym(dlhandle, "argsParser");
		enqueueKernel=dlsym(dlhandle, "enqueueKernel");

		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		int err;

			err=(*argsParser)(comm, l_selTask, workDim , globalThreads, localThreads, fmt, argsList);
			err|=(*enqueueKernel)(l_numTasks,l_selTask,workDim, globalThreads, localThreads);


		//dlclose(dlhandle); //TODO: something wrong here memory leak if I close!.
	return err;
}


//int _OMPI_P_XclExecTask(MPI_Comm comm, int g_selTask, int workDim, size_t * globalThreads,
	//	size_t * localThreads, struct timeval tval_globalInit , const char * fmt, va_list argsList) {

int _OMPI_P_XclExecTask(void* Args){
/*	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


	//Select the appropriate local task if any.
	if (myRank == g_taskList[g_selTask].r_rank) {
		debug_print("...task %d requested in rank %d \n",g_selTask,myRank);
		int l_selTask= g_taskList[g_selTask].l_taskIdx;*/

	//1.- Unwrap the Args.
		struct Args_ExecTask_st * execTask_Args=(struct Args_ExecTask_st *)Args;
		MPI_Comm comm = execTask_Args->comm;
		int l_selTask =	execTask_Args->l_selTask;
		int workDim   =	execTask_Args->workDim;
		size_t* globalThreads =	execTask_Args->globalThreads;
		size_t * localThreads =	execTask_Args->localThreads;
		const char* fmt = execTask_Args->fmt;
		va_list argsList;
		va_copy(argsList, execTask_Args->argsList);
		va_end(argsList);

		//va_list argsList = execTask_Args->argsList;

	//2.-The task thread makes the calls to the components
		void *dlhandle;

		int (*argsParser)(MPI_Comm, int selTask, int workDim, size_t*, size_t*, const char *,
				va_list);
		char *error;

		dlhandle = dlopen("libkernelMgmt.so", RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		argsParser = dlsym(dlhandle, "argsParser");

		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		int err;
		err=argsParser(comm, l_selTask, workDim , globalThreads, localThreads, fmt, argsList);
		//dlclose(dlhandle); //TODO: something wrong here memory leak if I close!.



	return 0;
}


int _OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){

	int myRank;
	MPI_Comm_rank(comm, &myRank);
	int * l_tasks=NULL;
	int l_taskCounter=0;
	int i,j;
	int err;
	int * l_ids=NULL;
	int * tmp_l_ids=NULL;
	int l_wTskSize=0;


	for(i=0;i<numTasks;i++){
		if (myRank == g_taskList[taskIds[i]].r_rank){
			l_wTskSize++;
			tmp_l_ids=realloc(l_ids,l_wTskSize*sizeof(int));
			if (tmp_l_ids!=NULL){
				l_ids=tmp_l_ids;
			}
			l_ids[l_wTskSize-1]=g_taskList[taskIds[i]].l_taskIdx;
		}
	}
	if(l_wTskSize>0){ //Am I (the process thread) the owner of any task?
		//err = (*XclWaitFor)(l_wTskSize,l_ids, comm);
		void *dlhandle;

		int (*XclWaitFor)(pthread_t* thds, int l_numTasks, int* l_taskIds, MPI_Comm comm);
		void (*localSynch)(int l_numTasks, int* localIDs, MPI_Comm Comm);
		char *error;

		dlhandle = dlopen("libsynchMgmt.so", RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		//XclWaitFor = dlsym(dlhandle, "XclWaitFor");
		localSynch=dlsym(dlhandle,"localSynch");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
		//err = (*XclWaitFor)(thds, l_wTskSize,l_ids, comm);
		(*localSynch)(l_wTskSize,l_ids,comm);
		//dlclose(dlhandle);
	}

	//MPI_Barrier(comm);


	return MPI_SUCCESS;
}

int _OMPI_XclWaitAllTasks(MPI_Comm comm){
	void *dlhandle;

	int (*XclWaitAllTasks)(int l_numTasks,MPI_Comm comm);
	char *error;

	dlhandle = dlopen("libkernelMgmt.so", RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	XclWaitAllTasks = dlsym(dlhandle, "XclWaitAllTasks");

	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	return (*XclWaitAllTasks)(l_numTasks,comm);

}


