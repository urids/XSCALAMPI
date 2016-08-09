/*
 * IScheduler.hpp
 *
 *  Created on: Jul 23, 2016
 *      Author: uriel
 */

#ifndef ISCHEDULER_HPP_
#define ISCHEDULER_HPP_

class Scheduler {
	//virtual void ~Schduler(){};
public:
	virtual int matchMake(int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule)=0;
};

#endif /* ISCHEDULER_HPP_ */
