/*
 * hiddenComms.h
 *
 *  Created on: Feb 6, 2015
 *      Author: uriel
 */

#ifndef HIDDENCOMMS_H_
#define HIDDENCOMMS_H_

#define DEBUG 0

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <CL/cl.h>

#include "ompi/include/mpi.h"
#include "../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../multiDeviceMgmt/deviceMgmt/deviceExploration.h"
#include "../multiDeviceMgmt/deviceMgmt/devicesInitialization.h"

#include "mpiext_XCLFrame_c.h"

void sendXploreInfo(MPI_Comm mcomm);
void rcvXploreInfo(MPI_Comm mcomm);

#endif /* HIDDENCOMMS_H_ */
