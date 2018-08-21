/*
 * devicesInitialization.h
 *
 *  Created on: May 9, 2014
 *      Author: uriel
 */

#ifndef DEVICESINITIALIZATION_H_
#define DEVICESINITIALIZATION_H_

#include "deviceExploration.h"
#include <stdio.h>
#include "stdlib.h"
#include "CL/cl.h"
#include "Device.h"
#include "localDevices.h"
#include "../errorMgmt/errorHandling.h"


void devicesInitialization(CLxplorInfo* xpInfo);


#endif /* DEVICESINITIALIZATION_H_ */
