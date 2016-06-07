/*
 * poolManager.h
 *
 *  Created on: May 25, 2016
 *      Author: uriel
 */

#ifndef POOLMANAGER_H_
#define POOLMANAGER_H_

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "pthread.h"
#include <semaphore.h>
#include "../../TaskManager/Base/task.h" //To access the global variable l_numTasks


int i;//index Exclusive

typedef void* (*function_t)(void*);

typedef struct node_st{
	function_t function;
	void* Args;
	struct node_st *next;
}node_t;


typedef node_t* queue;


sem_t* prodConsSemphs;
int * exitSignal;
int * reader_Sts;
pthread_mutex_t* prodConsExcl;
pthread_t* threads;
queue* SubRoutinesQueues;


typedef struct syncArgs_st{
	pthread_barrier_t localBarrier;
	int num_l_tasks;
}synchArgs_t;


int err;
void insertThreads(int numThds);



#endif /* POOLMANAGER_H_ */
