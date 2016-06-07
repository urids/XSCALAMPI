
#ifndef __dvMgmt_H__
#define __dvMgmt_H__

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "ompi/include/mpi.h"
#include "../../../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../../../multiDeviceMgmt/deviceMgmt/deviceExploration.h"
#include "../../../multiDeviceMgmt/deviceMgmt/devicesInitialization.h"
#include "PUsMap.h"

void deviceXploration(CLxplorInfo * clXplr);
void initializeDevices(CLxplorInfo*  xplorInfo);
extern int g_numDevs;//global variable defined in initialized in dvInit.c

#endif //__dvMgmt_H__
