/*
 * Device.h
 *
 *  Created on: Apr 28, 2014
 *      Author: uriel
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "CL/cl.h"

#ifndef DEBUG
#define DEBUG  1
#endif

#ifndef PROFILE
#define PROFILE  1
#endif


#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)

#define profile_print(fmt, ...) \
        do { if (PROFILE) fprintf(stderr, " AT: %s:%d:%s(): \n " fmt, __FILE__, \
                                __LINE__, __func__, ##__VA_ARGS__); } while (0)


typedef struct Device_st {

	//CL Objects and memory buffers
	int status;
	cl_device_type dType;       	//device type
	cl_device_id deviceId;      	//device ID
	cl_context_properties platform; //device platform controller
	cl_context context;         	//context
	cl_command_queue queue;     	//command-queue

	cl_event eventObject;       	//event object
	cl_ulong kernelStartTime;   	//kernel start time
	cl_ulong kernelEndTime;     	//kernel end time
	ulong elapsedTime;         	//elapsed time in ms
	//cl_float *output;           	//output host buffer for verification

	// Context properties
	cl_context_properties cprops[3];

	int numRacks;
	//Double pointer means racks of trays when a device handles multiple tasks =)!!
	cl_mem ** memHandler; //handler for the memory object (both cpu or gpu mem)
	cl_ulong maxAllocMemSize;
	cl_ulong globalMemSize;



} Device;

typedef Device* cpudev;
typedef Device* gpudev;
typedef Device* acceldev;

// Number of CPU and GPU devices
int numDevices;
int numCPUDevices;
int numGPUDevices;

#endif /* DEVICE_H_ */
