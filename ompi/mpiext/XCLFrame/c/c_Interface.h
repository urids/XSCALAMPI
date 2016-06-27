/*
 * c_Interface.h
 *
 *  Created on: Apr 14, 2016
 *      Author: uriel
 */

#ifndef C_INTERFACE_H_
#define C_INTERFACE_H_

#define NON_DELEGATED  //TAG to identify non delegated functions.


#include <stdarg.h>
#include <unistd.h> //for getenv
#include "ompi/include/mpi.h"


#include "../TaskManager/Base/task.h"
#include "../TaskManager/Base/taskManager.h"

#include "../DataManager/Base/dataManager.h"
#include "../DataManager/DataCopy/CopyContainers/containers.h" // to access containerList declaration

#include "../Scheduling/Base/scheduling.h"

#include "../multiDeviceMgmt/deviceMgmt/localDevices.h"

enum copyMode_en{
	INTRADEVICE=0,
	INTERDEVICE=1,
	INTERNODE=2
};

#endif /* C_INTERFACE_H_ */
