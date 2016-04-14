/*
 * taskFunctions.h
 *
 *  Created on: May 13, 2014
 *      Author: uriel
 */

#ifndef TASKPROCEDURES_H_
#define TASKPROCEDURES_H_

#define DEBUG 0

#include "../Base/task.h"
#include "CL/cl.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <unistd.h>
#include <stdarg.h>
#include "string.h"

#include "ompi/include/mpi.h"
#include "../../multiDeviceMgmt/deviceMgmt/localDevices.h"
#include "../../DataManager/BufferMgmt/bufferFunctions.h"
#include "../../c/ErrorHandling/errorHandling.h"

#include "../Base/taskManager.h"

int enqueueKernel(int numTasks,int selTask, int workDim, const size_t* globalThreads, const size_t* localThreads);

#endif /* TASKPROCEDURES_H_ */
