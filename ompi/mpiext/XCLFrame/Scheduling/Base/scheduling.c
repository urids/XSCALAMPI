
#include "scheduling.h"
#define ROOT 0
//?? still works?
device_Task_Info* taskDevMap;
int numDecls;

int taskThreadsEnabled=0; //global var declared in taskManager.h
schedConfigInfo_t * taskDevList; //Global variable declared in scheduling.h

/*
int _OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){
	int numRanks, myRank, HostNamelen;
	int i,j,k; //indx variables.


	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);
	MPI_Get_processor_name(hostname, &HostNamelen);

	//TODO PUT something like cases or remove this call and do a direct substitution
	//cases will be for scheduling mode (static dynamic or manual).

	//Init scheduling search.
	void *dlhandle;


	// continue normal exec.
	createTaskList(devSelection,comm); //function defined here.

	return g_numTasks;
}*/



int selectScheduler(int configInputs, char* heuristicModel, char* benchStoragePath, int numTasks){
	int schedulingMode=configInputs & SCHEDMASK; //Static, Dynamic or Manual.
	void *schedulerLibhandle;
	int (*readAndParse)();
	char *error;
	int i,j; //indx variables.

	switch (schedulingMode) {
	case TASKFILE:
		schedulerLibhandle =dlopen("libmanualSched.so",RTLD_LAZY);
		if (!schedulerLibhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}
		readAndParse=dlsym(schedulerLibhandle, "readAndParse");
		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(1);
		}

		g_numTasks=(*readAndParse)(); //creates the taskDevList in MANUAL CASE
		break;

	case STATICSCHED:
	{

		if(!numTasks){
			perror("You must specify the number of tasks in the XCL_initTasks. \n");
			exit(-1);
		}
		g_numTasks=numTasks;

		Scheduler* (*newScheduler)(char* heuristicModel, char* benchStoragePath);
		int (*matchMake)(Scheduler* myScheduler, int NumTsk, int NumDvs,
				float * W, int * Adj,const int* SR,
				const int* AS, int* schedule);
		char *error;

		schedulerLibhandle = dlopen("libstaticSched.so",RTLD_LAZY);
		if (!schedulerLibhandle) {
			fputs(dlerror(), stderr);
			exit(-1);
		}

		newScheduler=dlsym(schedulerLibhandle, "newScheduler"); //new
		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(-1);
		}
		matchMake=dlsym(schedulerLibhandle, "matchMake");
		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(-1);
		}


		Scheduler* myScheduler;
		myScheduler=newScheduler(heuristicModel,benchStoragePath);


		int numDevs=_OMPI_CollectDevicesInfo(ALL_DEVICES,MPI_COMM_WORLD);

		int* adjMtx= calloc(numTasks*numTasks,sizeof(int));
		read_Adj(adjMtx, numTasks);

		float* W= malloc(numTasks*numDevs*sizeof(float));
		build_Wij(W,numTasks,numDevs,configInputs,benchStoragePath);


		int* mapSchedule=malloc(numTasks*sizeof(int));
		int res=matchMake(myScheduler,numTasks,numDevs,W,adjMtx,NULL,NULL,mapSchedule);

		printf("-----------%s----------\n",heuristicModel);
		printf("\n------SCHEDULE----------\n");
		for(i=0;i<g_numTasks;i++){
			printf("%d -- %d \n",i,mapSchedule[i]);
		}


		taskDevList=malloc(g_numTasks*sizeof(schedConfigInfo_t));
		for(i=0;i<g_numTasks;i++){
			taskDevList[i].g_taskId=i;
			taskDevList[i].rank=g_PUList[mapSchedule[i]].r_rank; //assuming g_PUList is sorted by g_PUIdx.
			//TODO: this assign must be done iff myRank has the device. (other wise must be NULL)
			taskDevList[i].mappedDevice= g_PUList[mapSchedule[i]].mappedDevice;
		}

	}

	break;


	case DYNAMICSCHED:
		break;
	case 0: //no scheduling specifications

		break;

	default:
		perror("scheduling mismatched options. \n"
				"there are multiple incompatible scheduling options in the command line =|");
		abort ();
	}

	fillLocalTaskList(MPI_COMM_WORLD);
	fillGlobalTaskList(MPI_COMM_WORLD);
	insertThreads(l_numTasks,1); // At this very moment we known the local number of tasks.
	taskThreadsEnabled=1;

	return 0;
}

