/*
 * localDevices.h
 *
 *  Created on: May 9, 2014
 *      Author: uriel
 */

#ifndef LOCALDEVICES_H_
#define LOCALDEVICES_H_
#include "Device.h"


extern cpudev*   cpu;
extern gpudev*   gpu;
extern acceldev* accel;

typedef struct PUInfo_st{
	int g_PUIdx;
	int l_PUIdx;
	int r_rank;
}PUInfo;


extern PUInfo* g_PUList; //defined in c_Interfcae.c
extern int g_numDevs; //defined in c_Interfcae.c


#endif /* LOCALDEVICES_H_ */

