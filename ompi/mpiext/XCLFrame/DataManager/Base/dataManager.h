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
#include "../../c/taskMap.h"
#include "entityInfo.h"

//to access the queue mutex.
//Defined and static initialized in c_Interface.c
#include "../../c/queueMutexes.h"

/*
int _OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm);

int _OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm);

int _OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm);

int _OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm);

int _OMPI_XclSendRecv(int g_src_task, int src_trayIdx,
		             int g_dest_task, int dest_trayIdx,
		             int count, MPI_Datatype MPIentityType,
		             MPI_Comm comm);*/

int _OMPI_XclSend(void* Args);
int _OMPI_XclRecv(void* Args);
int _OMPI_XclReadTray(void* Args);
int _OMPI_XclWriteTray(void* Args);
int _OMPI_XclMallocTray(void* Args);
int _OMPI_XclFreeTray(void* Args);
int _interDevCpyProducer(void* Args);
int _interDevCpyConsumer(void* Args);
int _intraDevCpyProducer(void* Args);
int _intraDevCpyConsumer(void* Args);


struct Args_Send_st{
	int trayIdx;
	int count;
	MPI_Datatype MPIentityType;
	int l_src_task;
	int g_dest_task;
	int tgID;
	MPI_Comm comm;
};

struct Args_Recv_st{
	int trayIdx;
	int count;
	MPI_Datatype MPIentityType;
	int g_src_task;
	int l_recv_task;
	int tgID;
	MPI_Comm comm;

};

struct Args_ReadTray_st{
	int l_taskIdx;
	int trayIdx;
	int bufferSize;
	void * hostBuffer;
};

struct Args_WriteTray_st{
	int l_taskIdx;
	int trayIdx;
	int bufferSize;
	void * hostBuffer;
};

struct Args_MallocTray_st{
	int l_taskIdx;
	int trayIdx;
	int bufferSize;
};

struct Args_FreeTray_st{
	int l_taskIdx;
	int trayIdx;
};

struct Args_SendRecv_st{
	int src_task; //globals are used to choose the strategy
//	int l_src_task;
	int src_trayIdx;
	int dest_task; //globals are used to choose the strategy
//	int l_dest_task;
	int dest_trayIdx;
	int count;
	MPI_Datatype MPIentityType;
	MPI_Comm comm;
};



typedef struct ticket_st{
	pthread_mutex_t mtx;
	pthread_cond_t cond;
}ticket_t;


struct Args_matchedProducer_st{
	int tgID;
	sem_t* FULL;
	int l_taskIdx;
	int trayId;
	int dataSize;
	ticket_t* ticket;
};

struct Args_matchedConsumer_st{
	int tgID;
	sem_t* FULL;
	int l_taskIdx;
	int trayId;
	int dataSize;
	ticket_t* ticket;
};

struct Args_IntraMatchedConsumer_st{
	int tgID;
	sem_t* FULL;
	int l_src_taskIdx;
	int l_dst_taskIdx;
	int src_trayId;
	int dst_trayId;
	int dataSize;
	ticket_t* ticket;
};




#endif /* DATAMANAGER_H_ */
