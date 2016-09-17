
#include "wrapperFactory.h"
#include "IScheduler.hpp"
#include <exception>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "./ISH/ISH.h" //to construct the ISH instance
#include "./HEFT/HEFT.h" //to construct the HEFT instance
#include "Utilities/utilities.h"
extern "C" {

class myexception: public exception
{
  virtual const char* what() const throw()
  {
    return "My exception happened";
  }
} myex;

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

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
