/*
 * deviceExploration.h
 *
 *  Created on: May 9, 2014
 *      Author: uriel
 */

#ifndef DEVICEEXPLORATION_H_
#define DEVICEEXPLORATION_H_

#include "CL/cl.h"

typedef struct CLxplorInfo_st{
	int numDevices;
	int numCPUS;
	int numGPUS;
	int numACCEL;
	cl_platform_id* platforms;
	cl_uint platformCount;
	cl_device_id** devices;
}CLxplorInfo;


extern CLxplorInfo clXplr; //global variable defined in mpiext_XCLFrame_early.c


CLxplorInfo deviceExploration();

#endif /* DEVICEEXPLORATION_H_ */
