/*
 * bufferFunctions.h
 *
 *  Created on: May 11, 2014
 *      Author: uriel
 */

#ifndef BUFFERFUNCTIONS_H_
#define BUFFERFUNCTIONS_H_



#include <string.h>
#include <stdio.h>
#include <math.h>
#include "pthread.h"
#include "../../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../../TaskManager/Base/task.h"
#include "../../c/ErrorHandling/errorHandling.h"


extern int numEntitiesInHost;
extern float entitiesPerTask;
extern int* tskbufferSize; //Device BufferSize

#endif /* BUFFERFUNCTIONS_H_ */
