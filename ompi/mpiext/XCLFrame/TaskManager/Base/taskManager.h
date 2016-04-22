/*
 * taskManager.h
 *
 *  Created on: Apr 12, 2016
 *      Author: uriel
 */

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <unistd.h>
#include <stdarg.h>
#include "string.h"
#include <CL/cl.h>

#include "task.h"

#include "pthread.h"

#include "ompi/include/mpi.h"

typedef struct taskInfo_st{
	int g_taskIdx;
	int l_taskIdx;
	int r_rank;
}taskInfo;


extern taskInfo* g_taskList; //defined in c_Interface.c
extern int g_numTasks; //defined in c_Interface.c




typedef struct enqueueArgs_st{
	size_t* globalThreads;
	size_t* localThreads;
	cl_command_queue th_queue;
	cl_kernel th_kernel;
	int workDim;
	int g_selTsk;
	//cl_event kernelProfileEvent;

}enqueueArgs_t;
enqueueArgs_t **th_Args;
pthread_t*  thds;


#define DEBUG 0
#define PROFILE 0

#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define profile_print(fmt, ...) \
        do { if (PROFILE) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#endif /* TASKMANAGER_H_ */
