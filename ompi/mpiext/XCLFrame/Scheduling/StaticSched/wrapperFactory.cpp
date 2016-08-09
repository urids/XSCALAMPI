
#include "wrapperFactory.h"
#include "IScheduler.hpp"
#include "ISH.h"

extern "C" {

Scheduler* newScheduler(char* heuristicModel){

	if (strcmp(heuristicModel,"ISH")==0){
		Scheduler* myScheduler;
		myScheduler=new ISH();
		return myScheduler;
	}
	/*if (strcmp(heuristicModel,"HEFT")==0){
		Scheduler* myScheduler;
		myScheduler=new HEFT();
		return myScheduler;
	}*/
}

int matchMake(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule){
	return myScheduler->matchMake(NumTsk, NumDvs, W, Adj, SR,AS, schedule);
}

}
