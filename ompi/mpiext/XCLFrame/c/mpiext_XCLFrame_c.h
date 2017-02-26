
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "CL/cl.h"

#include "../multiDeviceMgmt/deviceMgmt/Device.h"
#include "../multiDeviceMgmt/deviceMgmt/localDevices.h"
#include "../TaskManager/Base/task.h"

#include "../multiDeviceMgmt/deviceMgmt/deviceExploration.h"
#include "../multiDeviceMgmt/deviceMgmt/devicesInitialization.h"



#define DEBUG 0
// this function also forms part of the framework but is declared in DataManager/TwoSidedComms/XclScatter.h(just for fun!!)
//this function is declared in XclScatter.h (Why?)-To show it is possible?
// int OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype);
// ACTUALLY IS DECLRED WITHOUT OMPI_DECLSPEC.
//OMPI_DECLSPEC int OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype);
OMPI_DECLSPEC int XCL_CommitEntity(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype);


OMPI_DECLSPEC int XCL_GetNumDevices(int* numDevices, int devSelection, MPI_Comm comm); //this function returns global number of devices in the system
//OMPI_DECLSPEC int OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm); //first parameter for this represents "ALL" or "GPUs" this function returns

OMPI_DECLSPEC int XSCALA_Initialize(int argc, char ** argv);
OMPI_DECLSPEC int XSCALA_getNumTasks(int* numTasks, MPI_Comm comm);
OMPI_DECLSPEC int XSCALA_Finalize();


//OMPI_DECLSPEC int OMPI_XclCreateKernel(MPI_Comm comm,char* srcPath,char* kernelName);
OMPI_DECLSPEC int OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath,char* kernelName);
//OMPI_DECLSPEC int OMPI_XclExecKernel(MPI_Comm communicator,int selTask,int globalThreads, int localThreads,const char * fmt, ...) __attribute__((format (printf, 5, 6)));
OMPI_DECLSPEC int OMPI_XclExecTask(MPI_Comm communicator,int selTask,int workDim, size_t * globalThreads, size_t * localThreads,const char * fmt, ...) __attribute__((format (printf, 6, 7)));
//TODO: new Scatter (const char* datafileName, int* count, MPI_Datatype MPIentityType, int trayIdx, MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclScatter(const char* datafileName, int* count, MPI_Datatype MPIentityType, int trayIdx, MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclGather(int trayIdx, int count, MPI_Datatype MPIentityType,void **hostbuffer, const char* datafileName , MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclSend(int trayIdx, int count, MPI_Datatype MPIentityType, int src_task, int dest_task, int tgId, MPI_Comm comm); //TODO: offset parameter to from tray start
OMPI_DECLSPEC int OMPI_XclRecv(int trayIdx, int count, MPI_Datatype MPIentityType, int src_task, int dest_task, int tgId, MPI_Comm comm); //TODO: offset parameter to from tray start
//OMPI_DECLSPEC int OMPI_XclSendRecv(int g_src_task, int src_trayIdx, int g_dst_task, int dst_trayIdx, int count, MPI_Datatype MPIentityType, int TAG, MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclSendRecv(int g_src_task, int src_trayIdx, int g_dst_task, int dst_trayIdx, int traySize, int tgID);
OMPI_DECLSPEC int OMPI_XclReadTray(int taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclWriteTray(int taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm);
//, task_t const* * myTasks,int numTasks
OMPI_DECLSPEC int XclCreateNewTasks(task_t* task, int numTasks, int Rank, int DeviceType, int DeviceID, MPI_Comm comm);
//TODO: implement Free routines.
OMPI_DECLSPEC int OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm);
//OMPI_DECLSPEC int OMPI_XclFreeAllBuffers(int taskIdx, MPI_Comm comm);
//OMPI_DECLSPEC int OMPI_XclFreeTask(int taskIdx, MPI_Comm comm);
//OMPI_DECLSPEC int OMPI_XclFreeAllTasks(MPI_Comm comm);

OMPI_DECLSPEC int OMPI_XclWaitAllTasks(MPI_Comm comm);
OMPI_DECLSPEC int OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm);


/***************************************/
/*  TASK DEPENDENCY TRACKING FUNCTIONS */
/***************************************/

OMPI_DECLSPEC int OMPI_XclTaskDependency(const int src_task, const int src_trayIdx, const int dest_task, const int dest_trayIdx, MPI_Comm comm);

