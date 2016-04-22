
#include "taskManager.h"

XCLtask* l_taskList; // Global Variable declared in task.h
int l_numTasks;//Global variable declared in task.h

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
	if(l_wTskSize>0){ //Am I owner of a task?
		//err = (*XclWaitFor)(l_wTskSize,l_ids, comm);
		void *dlhandle;

				int (*XclWaitFor)(pthread_t* thds, int l_numTasks, int* l_taskIds, MPI_Comm comm);
				char *error;

				dlhandle = dlopen("libsynchMgmt.so", RTLD_LAZY);
				if (!dlhandle) {
					fputs(dlerror(), stderr);
					exit(1);
				}

				XclWaitFor = dlsym(dlhandle, "XclWaitFor");

				if ((error = dlerror()) != NULL) {
					fputs(error, stderr);
					exit(1);
				}
				err = (*XclWaitFor)(thds, l_wTskSize,l_ids, comm);
				//dlclose(dlhandle);
	}

	MPI_Barrier(comm);


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


