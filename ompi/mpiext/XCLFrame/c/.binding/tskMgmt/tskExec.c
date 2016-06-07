
#include "tskMgmt.h"
#include "../../../TaskManager/Base/taskManager.h"
#define DEBUG 1

/* int XclExecKernel(MPI_Comm communicator, const char * fmt, ...)
    __attribute__((format (printf, 2, 3)));*/

//#define OMPI_XclExecKernel(dataFilePath , fmt, ...) _OMPI_XclExecKernel(dataFilePath , fmt,  ##__VA_ARGS__)

// int XclExecKernel(MPI_Comm comm, int selTask, int globalThreads, int localThreads, const char * fmt, va_list arg) {

int XclExecKernel(MPI_Comm comm, int selTask,int workDim , size_t * globalThreads, size_t* localThreads, const char * fmt, va_list arg) {
	int numParam = 0;
	const char *p;
	int j, k; //index exclusive
	int i;
	int tray;
	double dd;
	float f;
	unsigned u;
	char *s;

	cl_mem* memObj=malloc(sizeof(cl_mem));

	void *dlhandle;
	int (*setKernelArgs)(int, int, int, void*);
	int (*setKernelmemObj)(int,int,int,int);
	int (*enqueueKernel)(int l_numTasks,int selTask,int workDim ,const size_t* globalThreads, const size_t* localThreads);
	char *error;

	///home/uriel/Dev/mpiApps/FTWrkDistr/multiDeviceMgmt/Debug/
	dlhandle =dlopen("/home/uriel/mpiELFs/lib/XCLFrame/TaskManager/KernelMgmt/libkernelMgmt.so",RTLD_LAZY);

	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	setKernelArgs = dlsym(dlhandle, "setKernelArgs");
	setKernelmemObj=dlsym(dlhandle, "setKernelmemObj");
	enqueueKernel= dlsym(dlhandle,"enqueueKernel");

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}


  int numConsts=0;

	for(p = fmt; *p != '\0'; p++) {
		if(*p != '%') {
			continue;
		}
		switch(*++p) {
		case 'c':
			i = va_arg (arg, int);
			(*setKernelArgs)(selTask,numParam,sizeof(int),&i);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: int, arg number: %d, value: %d \n",numParam,i);
			break;

		case 'd':
			i = va_arg(arg, int);
			(*setKernelArgs)(selTask,numParam,sizeof(int),&i);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: int, arg number: %d, value: %d \n",numParam,i);
			break;
		case 'f':
			dd = va_arg(arg, double);
			f=(float)dd;
			(*setKernelArgs)(selTask,numParam,sizeof(float),&f);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: float, arg number: %d, value: %lf \n",numParam,f);
			break;

		case 's':
			s = va_arg(arg, char *);
			(*setKernelArgs)(selTask, numParam, sizeof(char*), s);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: string, arg number: %d, value: %s \n",numParam, s);
			break;
		case 'T':
			tray = va_arg(arg, int);
			(*setKernelmemObj)(selTask, numParam, sizeof(cl_mem), tray);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: cl_mem, arg number: %d, value: abstract  \n",numParam);
			break;

		case '%':
			putchar('%');
			break;
		}
	}

	(*enqueueKernel)(l_numTasks,selTask,workDim, globalThreads, localThreads);

  return 0;

}


int EXclWaitAllTasks(MPI_Comm comm){
 	void *dlhandle;

 			int (*XclWaitAllTasks)(int l_numTasks,MPI_Comm comm);
 			char *error;

 			dlhandle = dlopen("libtaskManager.so", RTLD_LAZY);
 			if (!dlhandle) {
 				fputs(dlerror(), stderr);
 				exit(1);
 			}

 			XclWaitAllTasks = dlsym(dlhandle, "XclWaitAllTasks");

 			if ((error = dlerror()) != NULL) {
 				fputs(error, stderr);
 				exit(1);
 			}
 			int err;
 			err = (*XclWaitAllTasks)(l_numTasks,comm);
 			dlclose(dlhandle);

 	 return MPI_SUCCESS;
 }

int XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){
	//global to local task's Id conversion.

	int myRank;
	MPI_Comm_rank(comm, &myRank);


	int * l_tasks=NULL;
	int l_taskCounter=0;
	int i,j;
/*
	for(i=0;i<numTasks;i++){
		j=taskIds[i];
		if(myRank==g_taskList[j].r_rank){
			//printf("rank %d starts to wait for gTask %d -> lTask %d\n",myRank,j,g_taskList[taskIds[i]].l_taskIdx);
			l_taskCounter++;

			int * tmp_l_task;
			tmp_l_task=realloc(l_tasks,l_taskCounter*sizeof(int)); //TODO: review not null pointer return.

			if (tmp_l_task != NULL) {
				l_tasks = tmp_l_task;
				l_tasks[l_taskCounter - 1] = g_taskList[j].l_taskIdx;
			} else {
				free(l_tasks);
				printf("Error AT:(re)allocating memory %s ,%s", __FILE__,__LINE__);
				exit(1);
			}
		}
	}
*/

	//finall function call to multiDeviceMgmt.
//	if(l_taskCounter > 0){
	if(numTasks>0){
		void *dlhandle;

		int (*XclWaitFor)(int l_numTasks, int* l_taskIds, MPI_Comm comm);
		char *error;

		dlhandle = dlopen("libtaskManager.so", RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		XclWaitFor = dlsym(dlhandle, "XclWaitFor");

		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
		int err;
		//err = (*XclWaitFor)(l_taskCounter,l_tasks, comm);
		err = (*XclWaitFor)(numTasks,taskIds, comm);
		dlclose(dlhandle);
	}


	return MPI_SUCCESS;


}
