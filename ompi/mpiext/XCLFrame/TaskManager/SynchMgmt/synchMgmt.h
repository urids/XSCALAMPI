/*
 * synchMgmt.h
 *
 *  Created on: Apr 14, 2016
 *      Author: uriel
 */

#ifndef SYNCHMGMT_H_
#define SYNCHMGMT_H_

#include "ompi/include/mpi.h"
#include "pthread.h"
#include "../Base/taskManager.h"

typedef struct syncArgs_st{
	pthread_barrier_t localBarrier;
	int num_l_tasks;
}synchArgs_t;
int i; //index Exclusive

#endif /* SYNCHMGMT_H_ */
