/*
 * scheduling.h
 *
 *  Created on: Apr 14, 2016
 *      Author: uriel
 */

#ifndef SCHEDULING_H_
#define SCHEDULING_H_

#include "dlfcn.h"
#include "stdlib.h"
#include "stdio.h"

#include "ompi/include/mpi.h"
#include "../../TaskManager/Base/taskManager.h"

#include "../../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../../multiDeviceMgmt/deviceMgmt/localDevices.h"
#include "../../multiDeviceMgmt/deviceMgmt/deviceExploration.h"

#include "../StaticSched/wrapperFactory.h"


typedef struct device_Task_Info_st{
 Device* mappedDevice;
 //int l_deviceIdx;
 int min_tskIdx;
 int max_tskIdx;
}device_Task_Info;

typedef struct schedConfigInfo_st{
	int g_taskId;
	int rank;
	Device* mappedDevice;
}schedConfigInfo_t;


extern device_Task_Info* taskDevMap;
extern int numDecls;

extern schedConfigInfo_t* taskDevList; //global variable defined in scheduling.c

//defined in taskSetup.c
extern int * myAssignedTasks; //this data is declared as extern because is used in
extern int* RKS; // collective operations.

typedef struct commsInfo_st{
  	float L,BW;
  	float* L_Mtx;
  	float* BW_Mtx;
 }commsInfo;

#define SCHEDMASK (0x7<<2)
#define DEFAULT_STATIC "HEFT"
#define DEFAULT_DYNAMIC "RR"

enum configOptions{
	TASKFILE=1<<4,
	STATICSCHED=1<<3,
	DYNAMICSCHED=1<<2,
	AUTOTUNE=1<<1,
	DEVICEFILTER=1<<0};

#endif /* SCHEDULING_H_ */
