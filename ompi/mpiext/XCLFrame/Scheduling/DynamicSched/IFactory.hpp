/*
 * IFactory.hpp
 *
 *  Created on: Sep 27, 2016
 *      Author: uriel
 */

#ifndef IFACTORY_HPP_
#define IFACTORY_HPP_

#include "IScheduler.hpp"
class Factory{
public:
	virtual Scheduler* newScheduler(char* heuristic, char * storagePath)=0;
};


#endif /* IFACTORY_HPP_ */
