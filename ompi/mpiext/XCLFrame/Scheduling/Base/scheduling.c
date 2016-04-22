
#include "scheduling.h"

device_Task_Info* taskDevMap;
int numDecls;
schedConfigInfo_t * taskDevList;

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
	int i,j; //index vars.

	void *dlhandle;
	int (*getNumDecls)();
	int (*readTaskBinding)(device_Task_Info* taskDevMap);

	//new
	int (*readAndParse)();

	char *error;
	dlhandle =dlopen("libmanualSched.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	getNumDecls = dlsym(dlhandle, "getNumDecls");
	readTaskBinding=dlsym(dlhandle, "readTaskBinding");


	readAndParse=dlsym(dlhandle, "readAndParse"); //new

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	//function defined in schedFileParse.c to get the number of declarations in the taskSched.cfg file.

/*	numDecls=getNumDecls();
	taskDevMap=malloc(numDecls*sizeof(device_Task_Info));

	for (i = 0; i < numDecls; i++) {
		taskDevMap[i].min_tskIdx=-1; //By default each device has no assigned task.
	}

	readTaskBinding(taskDevMap); //function defined in schedFileParse.c to read taskSched.cfg file.
	debug_print("1.- read taskDevMap succeed!!\n");

		taskSetup(devSelection); //this function fills the l_tasklist table.
*/
	//new
	(*readAndParse)();
	fillLocalTaskList(comm);
    fillGlobalTaskList(comm);
	return 0;
}



int _OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){
	int numRanks, myRank, HostNamelen;
		int i,j,k; //indx variables.


		char hostname[MPI_MAX_PROCESSOR_NAME];

		MPI_Comm_rank(comm, &myRank);
		MPI_Comm_size(comm,&numRanks);
		MPI_Get_processor_name(hostname, &HostNamelen);
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
