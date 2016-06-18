/*
 * dataCopy.h
 *
 *  Created on: Jun 15, 2016
 *      Author: uriel
 */

#ifndef DATACOPY_H_
#define DATACOPY_H_


#include <string.h>
#include <stdio.h>
#include <math.h>
#include "pthread.h"
#include "../../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../../TaskManager/Base/task.h"
#include "../../c/ErrorHandling/errorHandling.h"
#include "../../c/queueMutexes.h"

int readBuffer(int taskId,int trayIdx, int bufferSize, void * hostBuffer);

#endif /* DATACOPY_H_ */
