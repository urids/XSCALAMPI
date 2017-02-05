/*
 * defaultFactory.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: uriel
 */

#include "defaultFactory.h"
#include "../IScheduler.hpp"
#include <exception>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

defaultFactory::defaultFactory() {
	// TODO Auto-generated constructor stub

}

defaultFactory::~defaultFactory() {
	// TODO Auto-generated destructor stub
}


Scheduler* defaultFactory::newScheduler(char* heuristic, char * storagePath){
	if (strcmp(heuristic,"RR")==0){
			Scheduler* myScheduler;
			myScheduler=new RR();
			return myScheduler;
		}
	// Put here more heuristics for example EFT
	/*if (strcmp(heuristic,"EFT")==0){
				Scheduler* myScheduler;
				myScheduler=new EFT();
				return myScheduler;
	}*/
}

extern "C"{
Factory* factoryConstructor(){
	return new defaultFactory();
}
}
