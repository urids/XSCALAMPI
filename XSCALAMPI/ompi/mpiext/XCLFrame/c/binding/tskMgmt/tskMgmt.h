/*
 * tskMgmt.h
 *
 *  Created on: Feb 9, 2015
 *      Author: uriel
 */

#ifndef TKMGMT_H_
#define TKMGMT_H_


#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <dlfcn.h>
#include <CL/cl.h>



#include "ompi/include/mpi.h"

#include "../../../multiDeviceMgmt/taskMgmt/task.h"

#include "../../../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../../../multiDeviceMgmt/deviceMgmt/localDevices.h"
#include "../../../multiDeviceMgmt/deviceMgmt/deviceExploration.h"
#include "../../../multiDeviceMgmt/deviceMgmt/devicesInitialization.h"
#include "deviceMap.h"
#include "../../taskMap.h"

//#include "../../singleSidedComms/XclScatter.h"

extern int l_numTasks;  //This will be defined in late.c and is property of the runtime system.

#endif /* TKMGMT_H_ */
