/*
 * ISH.h
 *
 *  Created on: Jul 23, 2016
 *      Author: uriel
 */

#ifndef ISH_H_
#define ISH_H_
#include <stack> //for topological sorting.
#include <list> //for topological sorting.
#include <vector> //for topological sorting.
#include <algorithm> // for for_each
#include <map>
#include <iostream>
#include <assert.h>
#include <string.h>
#include "../IScheduler.hpp"
#include "../Utilities/utilities.h"




class ISH: public Scheduler {
public:
	ISH();
	virtual ~ISH();
	virtual int matchMake(int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);
};

#endif /* ISH_H_ */
