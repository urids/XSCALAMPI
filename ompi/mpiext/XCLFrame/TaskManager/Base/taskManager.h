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

#include "ompi/include/mpi.h"

typedef struct taskInfo_st{
	int g_taskIdx;
	int l_taskIdx;
	int r_rank;
}taskInfo;


extern taskInfo* g_taskList; //defined in late.c


extern int l_numTasks; //This  variable is defined in late.c and is property of the runtime system.

#define DEBUG 0
#define PROFILE 0

#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define profile_print(fmt, ...) \
        do { if (PROFILE) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#endif /* TASKMANAGER_H_ */
