

#include "taskManager.h"
#define DEBUG 1

int _OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath,char* kernelName){
	int myRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
		//Select the appropriate local task if any.
		if (myRank == g_taskList[g_selTask].r_rank) {
			int i;
			int l_selTask= g_taskList[g_selTask].l_taskIdx;

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
				int err; //TODO: l_numTasks must be deleted is no longer useful =/.
				err=(*createProgram)(l_selTask, srcPath,l_numTasks);
				err|=(*buildProgram)(l_selTask, l_numTasks);

				err|=(*createKernel)(l_selTask, kernelName, l_numTasks);
				err|=(*kernelXplor)(l_selTask, l_numTasks);


			dlclose(dlhandle);


			/*
			void *dlhandle;
			int (*XclCreateKernel)(MPI_Comm comm, int l_selTask, char* srcPath,char* kernelName,int l_numTasks);
			char *error;

			dlhandle =dlopen("libtskMgmt.so",RTLD_LAZY);
			if (!dlhandle) {
				fputs(dlerror(), stderr);
				exit(1);
			}

			XclCreateKernel = dlsym(dlhandle, "XclCreateKernel");

			if ((error = dlerror()) != NULL ) {
				fputs(error, stderr);
				exit(1);
			}

			int err;
			//take care here because clXplr will become the global and unique xploreInfo "object" maybe I should make it const

			//TODO: this is only if devSelection=ALL_DEVICES; otherwise maybe we need a switch.
			err=(*XclCreateKernel)(comm, l_selTask, srcPath,kernelName,l_numTasks);
			//err|=(*XclCreateKernel)(comm,srcPath,kernelName,& clXplr,gpu,&taskSet);
			//err|=(*XclCreateKernel)(comm,srcPath,kernelName,& clXplr,accel,&taskSet);
			dlclose(dlhandle);*/

		}
		return MPI_SUCCESS;
}




int _OMPI_XclExecTask(MPI_Comm comm, int g_selTask, int workDim, size_t * globalThreads,
		size_t * localThreads, const char * fmt, va_list argsList) {

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


	//Select the appropriate local task if any.
	if (myRank == g_taskList[g_selTask].r_rank) {
		debug_print("...task %d requested in rank %d \n",g_selTask,myRank);
		int l_selTask= g_taskList[g_selTask].l_taskIdx;

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
	}
	return MPI_SUCCESS;
}

int _OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){
 return 0;
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
