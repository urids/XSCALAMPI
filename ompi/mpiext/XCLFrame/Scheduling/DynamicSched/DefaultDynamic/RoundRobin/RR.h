/*
 * roundrobin.h
 *
 *  Created on: Sep 23, 2016
 *      Author: uriel
 */

#ifndef RR_H_
#define RR_H_

//#include "../../Base/scheduling.h"

#include <stack> //for topological sorting.
#include <list> //for topological sorting.
#include <vector> //for topological sorting.
#include <algorithm> // for for_each
#include <map>
#include <iostream>
#include <assert.h>
#include <string.h>
#include "../../IScheduler.hpp"


class RR: public Scheduler {
public:
	RR();
	virtual ~RR();
	virtual int matchMake(int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);

};


#endif /* ROUNDROBIN_H_ */
