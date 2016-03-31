
#include "ompi_config.h"

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdarg.h>

#include "ompi/mpi/c/bindings.h"
#include "ompi/mpiext/mpiext.h"
#include "hiddenComms.h"
#include "ompi/mpiext/XCLFrame/c/mpiext_XCLFrame_c.h"
#include "taskMap.h"
#include "binding/dvMgmt/PUsMap.h"
#include "../multiDeviceMgmt/deviceMgmt/deviceExploration.h"

/*
 * Here we put the definition of variables that can be queried in the user application
 * through an API call =)!!
 */

/* ==================================
 * | INIT OF GLOBAL INITIALIZATIONS |
 * ==================================
 * */

PUInfo* g_PUList; //Global Variable declared at PUsMap.h
int g_numDevs;  //Global Variable declared at PUsMap.h
XCLtask* taskList; // Global Variable declared in task.h
taskInfo* g_taskList; //Global Variable declared at taskMap.h
int l_numTasks;//Global variable declared in tskMgmt.h


/* ==================================
 * | END OF GLOBAL INITIALIZATIONS  |
 * ==================================
 * */

int numRanks, myRank, HostNamelen;


int OMPI_CollectDevicesInfo(int devSelection, MPI_Comm comm){

int i,j,k;
int myRank,numRanks;
int l_numDevs;


	l_numDevs=clXplr.numDevices;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);

	int* DPRKS =(int*)malloc(numRanks*sizeof(int)); //DPRKS-> Devices Per RanK Structure
	MPI_Allgather(&l_numDevs,1,MPI_INT,DPRKS,1,MPI_INT,comm);

	for(g_numDevs=0,i=0;i<numRanks;i++){
		g_numDevs+=DPRKS[i];
	}

 //this section creates the global PUs map structure.
	g_PUList=(PUInfo*)malloc(g_numDevs*sizeof(PUInfo)); //TODO: find the space for deallocation.

	for(i=0;i<g_numDevs;i++)
		g_PUList[i].g_PUIdx=i;

	for(i=0,k=0;i<numRanks;i++){
		for(j=0;j<DPRKS[i];j++){
			g_PUList[k].r_rank=i;
			g_PUList[k].l_PUIdx=j;
			k++;
		}
	}

	return g_numDevs;
}

//testOK

int OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){
	int numRanks, myRank, HostNamelen;
	int i,j,k; //indx variables.


	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);
	MPI_Get_processor_name(hostname, &HostNamelen);


	void *tskMgmt_dlhandle;
	int (*createTaskList)(int);
	char *error;
	tskMgmt_dlhandle = dlopen ("libtskMgmt.so", RTLD_LAZY);

	if (!tskMgmt_dlhandle ) {
			fputs(dlerror(), stderr);
			exit(1);
		}

	createTaskList = dlsym(tskMgmt_dlhandle, "createTaskList");
		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(1);
		}

	(*createTaskList)(devSelection); //function defined in tskMgmt.c

	dlclose(tskMgmt_dlhandle);



	int* RKS =(int*)malloc(numRanks*sizeof(int)); //RKS-> RanK Structure
	MPI_Allgather(&l_numTasks,1,MPI_INT,RKS,1,MPI_INT,comm);

	int g_numTasks;
	for(g_numTasks=0,i=0;i<numRanks;i++){
		g_numTasks+=RKS[i];
	}

	g_taskList=(taskInfo*)malloc(g_numTasks*sizeof(taskInfo)); //TODO: find the space for deallocation.

	for(i=0;i<g_numTasks;i++)
		g_taskList[i].g_taskIdx=i;


	 //this section creates the global Rank map structure.
	for(i=0,k=0;i<numRanks;i++){
		for(j=0;j<RKS[i];j++){
			g_taskList[k].r_rank=i;
			g_taskList[k].l_taskIdx=j;
			k++;
		}
	}

	return g_numTasks;
}


int OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath, char* kernelName){
	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	//Select the appropriate local task if any.
	if (myRank == g_taskList[g_selTask].r_rank) {
		int i;
		int l_selTask= g_taskList[g_selTask].l_taskIdx;
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
		dlclose(dlhandle);
	}
	return MPI_SUCCESS;
}

//int OMPI_XclExecKernel(MPI_Comm communicator, int g_selTask, int globalThreads,
	//int localThreads, const char * fmt, ...) {

int OMPI_XclExecTask(MPI_Comm communicator, int g_selTask, int workDim, size_t * globalThreads,
		size_t * localThreads, const char * fmt, ...) {

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


	//Select the appropriate local task if any.
	if (myRank == g_taskList[g_selTask].r_rank) {
		debug_print("...task %d requested in rank %d \n",g_selTask,myRank);
		int l_selTask= g_taskList[g_selTask].l_taskIdx;

		void *dlhandle;

		int (*XclExecKernel)(MPI_Comm, int selTask, int workDim, size_t*, size_t*, const char *,
				va_list);
		char *error;

		dlhandle = dlopen("libtskMgmt.so", RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		XclExecKernel = dlsym(dlhandle, "XclExecKernel");

		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
		int err;

		va_list argptr;
		va_start(argptr, fmt);
		err = (*XclExecKernel)(communicator, l_selTask, workDim ,globalThreads,
				localThreads, fmt, argptr);
		va_end(argptr);

		dlclose(dlhandle);
	}
	return MPI_SUCCESS;
}

int OMPI_XclWaitAllTasks(MPI_Comm comm){
	void *dlhandle;

	int (*XclWaitAllTasks)(MPI_Comm comm);
	char *error;

	dlhandle = dlopen("libtskMgmt.so", RTLD_LAZY);
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
	err = (*XclWaitAllTasks)(comm);

	dlclose(dlhandle);
	return MPI_SUCCESS;
}

 int OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){
	int i; //index variable
	int err;
	int * l_ids=NULL;
	int * tmp_l_ids=NULL;
	int l_wTskSize=0;
	void *dlhandle;
	MPI_Comm_rank(comm, &myRank);
	int (*XclWaitFor)(int numTasks, int* taskIds, MPI_Comm comm);
	char *error;

	dlhandle = dlopen("libtskMgmt.so", RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	XclWaitFor = dlsym(dlhandle, "XclWaitFor");

	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}

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
		err = (*XclWaitFor)(l_wTskSize,l_ids, comm);
	}

	MPI_Barrier(comm);

	dlclose(dlhandle);
	return MPI_SUCCESS;
 }
