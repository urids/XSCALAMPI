/*
 * deviceExploration.c
 *
 *  Created on: May 9, 2014
 *      Author: uriel
 */

#include "CL/cl.h"
#include "stdio.h"
#include "deviceExploration.h"

CLxplorInfo deviceExploration() {
	CLxplorInfo cx;

	int i, j;
	int numCPUS = 0;
	int numGPUS = 0;
	int numACCEL = 0;
	char* devType;
	size_t devTypeSize;
	cl_uint platformCount;
	cl_platform_id* platforms;
	cl_uint deviceCount;
	cl_device_id** devices;


	// get all platforms
	clGetPlatformIDs(0, NULL, &platformCount);
	platforms = (cl_platform_id*) malloc(
			sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);

	devices = (cl_device_id**) malloc(sizeof(cl_device_id*) * platformCount);

	for (i = 0; i < platformCount; i++) {

		// get all devices
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
		devices[i] = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount,
				devices[i], NULL);

		for (j = 0; j < deviceCount; j++) {

			/***  i = platform & j = device  ***/

			clGetDeviceInfo(devices[i][j], CL_DEVICE_TYPE, 0, NULL,
					&devTypeSize);
			devType = (char*) malloc(devTypeSize);
			clGetDeviceInfo(devices[i][j], CL_DEVICE_TYPE, devTypeSize,
					&devType, NULL);

			if (CL_DEVICE_TYPE_CPU == devType) {
				numCPUS++;
			}
			if (CL_DEVICE_TYPE_GPU == devType) {
				numGPUS++;
			}
			if (CL_DEVICE_TYPE_ACCELERATOR == devType) {
				numACCEL++;
			}
		}
	}

	cx.platforms= platforms;
	cx.platformCount=platformCount;
	cx.numDevices=numCPUS+numGPUS+numACCEL;
	cx.numCPUS=numCPUS;
	cx.numGPUS=numGPUS;
	cx.numACCEL=numACCEL;
	cx.devices=devices;

	return cx;
}
