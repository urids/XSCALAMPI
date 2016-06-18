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
#include <semaphore.h>

#include <sys/time.h> //for profiling =)

//to access the queue mutex.
//Defined and static initialized in c_Interface.c
#include "../../c/queueMutexes.h"
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

extern struct timeval tval_globalInit; //defined in c_Interface.c

extern int taskThreadsEnabled; //defined and init in scheduling.c

typedef struct enqueueArgs_st{
	size_t* globalThreads;
	size_t* localThreads;
	cl_command_queue th_queue;
	cl_kernel th_kernel;
	int workDim;
	int g_selTsk;
	//cl_event kernelProfileEvent;

}enqueueArgs_t;

//enqueueArgs_t **th_Args; becomes local in kernelSetup.c
//pthread_t*  thds; //no longer used.


#define DEBUG 0
#define PROFILE 0

#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define profile_print(fmt, ...) \
        do { if (PROFILE) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)


//in taskManager.c (the interface)
int _OMPI_XclSetProcedure(void* Args);
int _OMPI_XclExecTask(void* Args);
int _OMPI_P_XclExecTask(void* Args);


//in syncMgmt.c (the module)
void* waitSubroutine(void*Args);
void* signalSubroutine(void*Args);

//////////////////////////////////////////////////////////////////////////////////
////////////////////// 	INIT SUBROUTINES ARGS DECLARATIONS////////////////////////

struct Args_SetProcedure_st{
	//MPI_Comm comm;
	int l_selTask;
	char* srcPath;
	char* kernelName;
};

struct Args_ExecTask_st{
	MPI_Comm comm;
	int l_selTask;
	int workDim;
	size_t* globalThreads;
	size_t* localThreads;
	const char* fmt;
	va_list argsList;
};

struct Args_waitSubroutine_st{
	sem_t* semaphore;
};

struct Args_signalSubroutine_st{
	sem_t* semaphore;
};

////////////////////// 	END SUBROUTINES ARGS DECLARATIONS////////////////////////
//////////////////////////////////////////////////////////////////////////////////



#endif /* TASKMANAGER_H_ */
