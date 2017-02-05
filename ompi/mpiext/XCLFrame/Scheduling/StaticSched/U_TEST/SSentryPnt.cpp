#include "common.h"

extern "C"{

int dummy(int a);

void entrypSS(void)  //entry is the entry point instead of main
{

	//int numTasks;
	//int *adjMtx=newRandDAG( /*OUT*/ &numTasks);

int i,j;

	execEnvironment randSys;

	randApplication randDAG;


/*	//Print DAG
	for(i=0;i<randDAG.numTasks;i++){
		for(j=0;j<randDAG.numTasks;j++){
			printf(" %d ", randDAG.EadjMtx[randDAG.numTasks*i+j]);
		}
		printf(" \n");
	}*/




	float* W0=(float*)malloc(randDAG.numTasks*sizeof(float));
	for(i=0;i<randDAG.numTasks;i++){
		W0[i]=gaussianNumber(100,0.2*100);
		//printf(" %d ",(int)W0[i]);
	}

	float* W=(float*)malloc(randDAG.numTasks*randSys.numDevices*sizeof(float));
	for(i=0;i<randDAG.numTasks;i++){
		int dvc=i%randSys.numDevices;
		for(j=0;j<randSys.numDevices;j++){
			W[randSys.numDevices*i+j]=W0[i]*randSys.AccMtx[randSys.numDevices*dvc+j];
			//printf(" %d ",(int)W[randSys.numDevices*i+j]);
		}
		//printf(" \n");
	}


	/*for_each(randSys.PUs.begin(),randSys.PUs.end(),[](PU_t device){
		printf(" %d \n", device.ID);
	});*/


	//int numDevices;
	//PU_t* rsrcList=newRsrcList(/*OUT*/ &numDevices);
	//float *BWMtx;

	int* mapSchedule = (int*)malloc(randDAG.numTasks*sizeof(int));
	Scheduler* testScheduler= newScheduler("HEFT", randSys.U_TEST_Path);

	int res=matchMake(testScheduler,randDAG.numTasks,randSys.numDevices,W,randDAG.EadjMtx,NULL,NULL,mapSchedule);
	fprintf(stderr,"\n numTasks: %d \n", dummy(randDAG.numTasks));
	fprintf(stderr,"\n numDevices: %d \n", dummy(randSys.numDevices));
	fprintf(stderr,"\n numRanks: %d \n", dummy(randSys.numRanks));

	printf("\n------SCHEDULE----------\n");
	for(i=0;i<randDAG.numTasks;i++){
		printf("%d -- %d \n",i,mapSchedule[i]);
	}


	_exit(0);
}

}// END_extern_"C"
