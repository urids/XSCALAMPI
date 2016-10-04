/*
 * defaultFactory.h
 *
 *  Created on: Sep 27, 2016
 *      Author: uriel
 */

#ifndef DEFAULTFACTORY_H_
#define DEFAULTFACTORY_H_
#include "../IFactory.hpp"
#include "../IScheduler.hpp"
#include <exception>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


#include "./RoundRobin/RR.h" //to construct the RR instance

class defaultFactory : public Factory {
public:
	defaultFactory();
	virtual ~defaultFactory();
	virtual Scheduler* newScheduler(char* heuristic, char * storagePath);
};

#endif /* DEFAULTFACTORY_H_ */
