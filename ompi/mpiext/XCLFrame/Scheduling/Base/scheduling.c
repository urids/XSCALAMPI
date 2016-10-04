
#include "scheduling.h"
#define ROOT 0

device_Task_Info* taskDevMap;
int numDecls;

int taskThreadsEnabled=0; //global var declared in taskManager.h
schedConfigInfo_t * taskDevList; //Global variable declared in scheduling.h



int selectScheduler(int configInputs, char* heuristicModel, char* benchStoragePath){
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
		int numTasks;
		read_Adj(NULL, &numTasks);
		g_numTasks=numTasks;

		printf("num tasks %d",g_numTasks);

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

		int* adjMtx= malloc(numTasks*numTasks*sizeof(int));
		read_Adj(adjMtx, &numTasks);

		float* W= malloc(numTasks*numDevs*sizeof(float));
		build_Wij(W,numTasks,numDevs,configInputs,benchStoragePath);


		int* mapSchedule=malloc(numTasks*sizeof(int));
		int res=matchMake(myScheduler,numTasks,numDevs,W,adjMtx,NULL,NULL,mapSchedule);

		/*printf("-----------%s----------\n",heuristicModel);
		printf("\n------SCHEDULE----------\n");
		for(i=0;i<g_numTasks;i++){
			printf("%d -- %d \n",i,mapSchedule[i]);
		}*/


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

		printf("no schedule specs: apply default\n");
		return 0;
		//break;

	default:
		perror("scheduling mismatched options. \n"
				"there are multiple incompatible scheduling options in the command line =|");
		abort ();
	}

	fillLocalTaskList(MPI_COMM_WORLD);
	fillGlobalTaskList(MPI_COMM_WORLD);
	// At this very moment we known the local number of tasks.
	insertThreads(l_numTasks,1); // we must reset the thread counter.
	taskThreadsEnabled=1;

	return 0;
}

int dynDistribution(task_t* task,  int numTasks, int INVOKER, int DeviceType, int DeviceID, MPI_Comm comm){
	int i;
	static int currNumTasks=0;
	char *error;
	void *schedulerLibhandle;

	static int* mapSchedule;
	int numDevs;

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	Scheduler* (*newScheduler)(char* heuristicModel, char* benchStoragePath);
	int (*matchMake)(Scheduler* myScheduler, int NumTsk, int NumDvs,
			float * W, int * Adj,const int* SR,
			const int* AS, int* schedule);

	numDevs =_OMPI_CollectDevicesInfo(ALL_DEVICES,MPI_COMM_WORLD);


	if(!currNumTasks){
		mapSchedule=malloc(numTasks*sizeof(int));

		if(myRank == INVOKER){
			//1.- LOAD THE COMPONENT
			schedulerLibhandle = dlopen("libdynamicSched.so", RTLD_LAZY);
			if (!schedulerLibhandle) {
				fputs(dlerror(), stderr);
				exit(1);
			}

			//2.- OPEN WRAPPER SYMBOLS
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

			//3.- INSTATNCE AN SCHEDULER AND REQUEST TO DO THE MAPPING
			Scheduler* myScheduler;
			myScheduler=newScheduler("RR",NULL); //factory request through the wrapper

			//4.-REQUEST THE MAPPING
			int chkerr=matchMake(myScheduler,numTasks,numDevs,NULL,NULL,NULL,NULL,mapSchedule);

		}


		MPI_Bcast(mapSchedule,numTasks,MPI_INT,INVOKER,comm);
		for(i=0;i<numTasks;i++){
			printf(" %d -- %d\n",i,mapSchedule[i]);
		}

		g_numTasks=numTasks;
		//FILL TASK-DEVLIST (MAP)
		taskDevList=malloc(g_numTasks*sizeof(schedConfigInfo_t));
		for(i=0;i<g_numTasks;i++){
			(task+i)->ID=i;
			taskDevList[i].g_taskId=i;
			taskDevList[i].rank=g_PUList[mapSchedule[i]].r_rank; //assuming g_PUList is sorted by g_PUIdx.
			//TODO: this assign must be done iff myRank has the device. (other wise must be NULL)
			taskDevList[i].mappedDevice= g_PUList[mapSchedule[i]].mappedDevice;
		}


		fillLocalTaskList(MPI_COMM_WORLD);
		fillGlobalTaskList(MPI_COMM_WORLD);
		insertThreads(l_numTasks,1); // we must reset the thread counter.
		taskThreadsEnabled=1;
	} else{	//not the first call
		mapSchedule=realloc(mapSchedule,(currNumTasks+numTasks)*sizeof(int));

		if(myRank == INVOKER){
			//1.- LOAD THE COMPONENT
			schedulerLibhandle = dlopen("libdynamicSched.so", RTLD_LAZY);
			if (!schedulerLibhandle) {
				fputs(dlerror(), stderr);
				exit(1);
			}

			//2.- OPEN WRAPPER SYMBOLS
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

			//3.- INSTATNCE AN SCHEDULER AND REQUEST TO DO THE MAPPING
			Scheduler* myScheduler;
			myScheduler=newScheduler("RR",NULL); //factory request through the wrapper

			//4.-REQUEST THE MAPPING
			int chkerr=matchMake(myScheduler,numTasks,numDevs,NULL,NULL,NULL,NULL,mapSchedule);

		}

		int newNumTsks=currNumTasks+numTasks;
		MPI_Bcast(mapSchedule,newNumTsks,MPI_INT,INVOKER,comm);


		for(i=0;i<newNumTsks;i++){
			printf(" %d -- %d\n",i,mapSchedule[i]);
		}

		g_numTasks=currNumTasks+numTasks;
		//RE-FILL TASK-DEVLIST (MAP)
		taskDevList=realloc(taskDevList,g_numTasks*sizeof(schedConfigInfo_t));
		for(i=currNumTasks;i<g_numTasks;i++){
			(task+i-currNumTasks)->ID=i;
			taskDevList[i].g_taskId=i;
			taskDevList[i].rank=g_PUList[mapSchedule[i]].r_rank; //assuming g_PUList is sorted by g_PUIdx.
			//TODO: this assign must be done iff myRank has the device. (other wise must be NULL)
			taskDevList[i].mappedDevice= g_PUList[mapSchedule[i]].mappedDevice;
		}
		int pre_l_numTasks=l_numTasks;
		printf("sched: l_numTsks: %d \n",l_numTasks);
		reallocLocalTaskList(currNumTasks,MPI_COMM_WORLD);
		printf("sched new l_numTsks: %d \n",l_numTasks);
		reallocGlobalTaskList(MPI_COMM_WORLD);

		if(pre_l_numTasks<l_numTasks){
			int moreTasks=l_numTasks-pre_l_numTasks;
			insertThreads(moreTasks, 0);
		}
	}  // END ELSE NOT FIRST CALL

	currNumTasks+=numTasks;
	return 0;
}
