
#include "scheduling.h"

//?? still works?
device_Task_Info* taskDevMap;
int numDecls;

int taskThreadsEnabled=0; //global var declared in taskManager.h
schedConfigInfo_t * taskDevList; //Global variable declared in scheduling.h

int _OMPI_CollectDevicesInfo(int devSelection, MPI_Comm comm){

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

int createTaskList(int devSelection,MPI_Comm comm){ //This function fills the l_taskList table
									  //There exist one l_taskList per Node
	int i,j; //index exclusive vars.
	void *dlhandle;
	int (*readAndParse)();

	char *error;
	dlhandle =dlopen("libmanualSched.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	readAndParse=dlsym(dlhandle, "readAndParse"); //new

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	// Major Steps //
	(*readAndParse)();
	fillLocalTaskList(comm);
    fillGlobalTaskList(comm);
    insertThreads(l_numTasks); // At this very moment we known the local number of tasks.
    taskThreadsEnabled=1;

	return 0;
}



int _OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){
	int numRanks, myRank, HostNamelen;
		int i,j,k; //indx variables.


		char hostname[MPI_MAX_PROCESSOR_NAME];

		MPI_Comm_rank(comm, &myRank);
		MPI_Comm_size(comm,&numRanks);
		MPI_Get_processor_name(hostname, &HostNamelen);

		//TODO PUT something like cases or remove this call and do a direct substitution
		//cases will be for scheduling mode (static dynamic or manual).

		//Init. the scheduling search.
		void *dlhandle;
		Scheduler* (*newScheduler)(char* heuristicModel);
		int (*matchMake)(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);
		char *error;
		dlhandle =dlopen("libstaticSched.so",RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		newScheduler=dlsym(dlhandle, "newScheduler"); //new
		matchMake=dlsym(dlhandle, "matchMake");
		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(1);
		}

		Scheduler* myScheduler;
		char heuristicModel[]="ISH";
		myScheduler=newScheduler(heuristicModel);

		int V=9;
		int numDvs=4;
		int* testAdj= calloc(V*V,sizeof(int)); //adjacency mtx are allways squared.
		float* testW= calloc(V,sizeof(float)); //Weight mtx

		testW[0]=2;
		testW[1]=3;
		testW[2]=3;
		testW[3]=4;
		testW[4]=5;
		testW[5]=4;
		testW[6]=4;
		testW[7]=4;
		testW[8]=1;

		float* W= malloc(V*numDvs*sizeof(float));
		for(i=0;i<V;i++){
			for(j=0;j<numDvs;j++){
				W[numDvs*i+j]=testW[i];
			}
		}

		testAdj[1]=4;
		testAdj[2]=1;
		testAdj[3]=1;
		testAdj[4]=1;
		testAdj[6]=10;
		testAdj[14]=1;
		testAdj[15]=1;
		testAdj[25]=1;
		testAdj[34]=1;
		testAdj[53]=5;
		testAdj[62]=6;
		testAdj[71]=5;

		int* mapSchedule=malloc(V*sizeof(int));
		for(i=0;i<V;i++){
			mapSchedule[i]=-1;
		}
		int res=matchMake(myScheduler,V,numDvs,W,testAdj,NULL,NULL,mapSchedule);

		printf("\n------------\n");
		for(i=0;i<V;i++){
			printf("%d -- %d \n",i,mapSchedule[i]);
		}

		// continue normal exec.
		createTaskList(devSelection,comm); //function defined here.

	return g_numTasks;
}

int GG_OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){ //this function fills the g_taskList table.
	int numRanks, myRank, HostNamelen;
	int i,j,k; //indx variables.


	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);
	MPI_Get_processor_name(hostname, &HostNamelen);
	createTaskList(devSelection,comm); //function defined here.


	int* RKS =(int*)malloc(numRanks*sizeof(int)); //RKS-> RanK Structure
	MPI_Allgather(&l_numTasks,1,MPI_INT,RKS,1,MPI_INT,comm);


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


int commsBenchmark(commsInfo* cmInf){
	void *dlhandle;
	void(*comsBench)(commsInfo* cmInf);

	char *error;

	dlhandle =dlopen("libbenchmark.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	comsBench = dlsym(dlhandle, "_commsBenchmark");

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}
	(*comsBench)(cmInf);

	return 0;
}

int compBenchmark(commsInfo* cmInf){
	return 0;
}

