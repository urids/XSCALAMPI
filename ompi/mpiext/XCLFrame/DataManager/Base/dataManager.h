/*
 * dataManager.h
 *
 *  Created on: Apr 13, 2016
 *      Author: uriel
 */

#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include "ompi/include/mpi.h"
#include "stdlib.h"
#include "stdio.h"
#include "dlfcn.h"

#include "../../TaskManager/Base/taskManager.h"

int _OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm);

int _OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm);

int _OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm);

int _OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm);

int _OMPI_XclSendRecv(int src_task, int src_trayIdx,
		             int dest_task, int dest_trayIdx,
		             int count, MPI_Datatype MPIentityType,
		             MPI_Comm comm);



#endif /* DATAMANAGER_H_ */
