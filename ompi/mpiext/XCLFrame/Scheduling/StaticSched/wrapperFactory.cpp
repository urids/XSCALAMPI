
#include "wrapperFactory.h"
#include "IScheduler.hpp"


#include "./ISH/ISH.h" //to construct the ISH instance
#include "./HEFT/HEFT.h" //to construct the HEFT instance
#include "Utilities/utilities.h"
extern "C" {

Scheduler* newScheduler(char* heuristicModel, char* benchStoragePath){ //factory.

	if (strcmp(heuristicModel,"ISH")==0){
		Scheduler* myScheduler;
		myScheduler=new ISH();
		return myScheduler;
	}
	if (strcmp(heuristicModel,"HEFT")==0){
		Scheduler* myScheduler;
		myScheduler=new HEFT(benchStoragePath);
		return myScheduler;
	}
}

int matchMake(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule){
	int i;
	int xtndNumTasks=NumTsk+2; // Aggregate init and end task.

	int* xtndAdjMtx=(int*)calloc(xtndNumTasks*xtndNumTasks,sizeof(int));
	buildExtendedAdj(xtndAdjMtx,Adj,NumTsk);

	float* xtndW=(float*)calloc(xtndNumTasks*NumDvs,sizeof(float));
	buildExtendedWij(xtndW,W,NumTsk,NumDvs);

	int* xtndMapSchedule=(int*)malloc(xtndNumTasks*sizeof(int));
	for(i=0;i<xtndNumTasks;i++){
		xtndMapSchedule[i]=-1;
	}

	myScheduler->matchMake(xtndNumTasks, NumDvs, xtndW, xtndAdjMtx, SR,AS, xtndMapSchedule);
	memcpy(schedule,xtndMapSchedule+1,NumTsk*sizeof(int));

	return 0;
}

}
