/*
 * HEFT.h
 *
 *  Created on: Aug 20, 2016
 *      Author: uriel
 */

#ifndef HEFT_H_
#define HEFT_H_
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

class HEFT: public Scheduler {
public:
	HEFT();
	HEFT(char* Path);
	virtual ~HEFT();
	virtual int matchMake(int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);

private:
  char* storagePath; //this implementation will require reading L & BW Matrices.
};

#endif /* HEFT_H_ */
