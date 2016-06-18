/*
 * collectiveOps.h
 *
 *  Created on: Jun 15, 2016
 *      Author: uriel
 */

#ifndef COLLECTIVEOPS_H_
#define COLLECTIVEOPS_H_

#include "stdlib.h"
#include "stdio.h"
#include "dlfcn.h"

#include "ompi/include/mpi.h"
#include "../../TaskManager/Base/taskManager.h"
#include "../Base/entityInfo.h"

extern int numEntitiesInHost;
extern float entitiesPerTask;
extern int* tskbufferSize; //Device BufferSize


#endif /* COLLECTIVEOPS_H_ */
