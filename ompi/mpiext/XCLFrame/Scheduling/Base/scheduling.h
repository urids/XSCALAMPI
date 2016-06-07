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

typedef struct commsInfo_st{
  	float L,BW;
  	float* L_Mtx;
  	float* BW_Mtx;
 }commsInfo;


#endif /* SCHEDULING_H_ */
