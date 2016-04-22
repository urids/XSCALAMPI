/*
 * task.h
 *
 *  Created on: May 7, 2014
 *      Author: uriel
 */

#ifndef TASK_H_
#define TASK_H_

#include "CL/cl.h"
#include "../../multiDeviceMgmt/deviceMgmt/Device.h"

//#include "taskProcedures.h"


typedef struct Xcode_st{
	char *source_str;
	size_t source_size;
}XCLcode;


typedef struct XTrayInfo_st{
	int bufferMode;
	size_t size;
	//void ** data;
}XCLTrayInfo;


typedef struct Xkernel_st{
	cl_kernel kernel;
	cl_uint numArgs;
	char* kernelName;
	//int** argsLengths;
	//void** argsValues;

}XCLkernel;

typedef struct XCLtask_st{

	int localID;


/*********************************************
				DEVICE
*********************************************/
	//every task MUST have an associated device
	Device* device;


/*********************************************
				PROCEDURE
*********************************************/
	//every task MUST have its code //and have their kernel List
	XCLcode* code;
	cl_program* CLprogram;
	int numKernels; // TODO: will be useful for multiKernels.
	XCLkernel* kernel;


/*********************************************
				DATA
*********************************************/

	int numTrays; //number of trays for this task
	XCLTrayInfo *trayInfo; /*info about size and mode of tray.
	 	 	 	 	 	 	 	 the current cl_mem object is managed by its device*/
	int Rack; //number Of Rack in device of this task. two task may have same number of rack
				 // provided that they belong to different devices =) !!

}XCLtask;


extern XCLtask* l_taskList; //This will is defined in taskManager.c and is property of the runtime system.
extern int l_numTasks; //This variable is defined in taskManager.c and is property of the runtime system.


#endif /* TASK_H_ */
