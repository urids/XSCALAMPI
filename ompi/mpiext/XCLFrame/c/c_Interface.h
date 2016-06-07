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

#include "../multiDeviceMgmt/deviceMgmt/localDevices.h"



#endif /* C_INTERFACE_H_ */
