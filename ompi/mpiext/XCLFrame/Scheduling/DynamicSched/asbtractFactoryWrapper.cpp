#include "../Base/schedWrapper.h"
#include "IFactory.hpp"
#include <exception>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


#include "dlfcn.h"

typedef Factory* (*factoryConstructor_t)();
//WRAPPER
extern "C" {
Scheduler* newScheduler(char* heuristicModel, char* benchStoragePath){

	void *FactoryHndlr;
	char *error;
	FactoryHndlr=dlopen("libdefaultDynamic.so", RTLD_LAZY);
	if (!FactoryHndlr) {
		fputs(dlerror(), stderr);
		exit(1);
	}
	Factory* (*factoryConstructor)();
	factoryConstructor= (factoryConstructor_t) dlsym(FactoryHndlr,"factoryConstructor");

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(-1);
	}

	//1.- Build the Factory
	Factory* myFactory= factoryConstructor();
	//2.- Build the Scheduler
	Scheduler* myScheduler=myFactory->newScheduler(heuristicModel,benchStoragePath);

}


int matchMake(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule){
	myScheduler->matchMake(NumTsk, NumDvs, W, Adj, SR, AS, schedule);
	return 0;
}

}
