

#include "c_Interface.h"

/* ==================================
 * | INIT OF GLOBAL INITIALIZATIONS |
 * ==================================
 * */

PUInfo* g_PUList; //Global Variable declared at localDevices.h
int g_numDevs;  //Global Variable declared at localDevices.h

taskInfo* g_taskList; //Global Variable declared at taskManager.h
int g_numTasks; //Global Variable declared at taskManager.h



/* ==================================
 * | END OF GLOBAL INITIALIZATIONS  |
 * ==================================
 * */

int OMPI_CollectDevicesInfo(int devSelection, MPI_Comm comm){
	return _OMPI_CollectDevicesInfo(devSelection, comm);
}


int OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){
	return _OMPI_CollectTaskInfo(devSelection, comm);
}


int OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath, char* kernelName){
	return _OMPI_XclSetProcedure(comm, g_selTask, srcPath, kernelName);
}


int OMPI_XclExecTask(MPI_Comm communicator, int g_selTask, int workDim, size_t * globalThreads,
		size_t * localThreads, const char * fmt, ...) {

	va_list argptr;
	va_start(argptr, fmt);

	 _OMPI_XclExecTask(communicator, g_selTask, workDim, globalThreads,
				localThreads, fmt, argptr);
	va_end(argptr);

	return 0;
}


int OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
	return _OMPI_XclWriteTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
}

int OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm){
	return _OMPI_XclReadTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
}

int OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){
	return _OMPI_XclMallocTray(g_taskIdx, trayIdx, bufferSize, comm);
}

int OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm){
	return _OMPI_XclFreeTray(g_taskIdx, trayIdx, comm);
}


int OMPI_XclSendRecv(int src_task, int src_trayIdx,
		             int dest_task, int dest_trayIdx,
		             int count, MPI_Datatype MPIentityType,
		             MPI_Comm comm){
	return _OMPI_XclSendRecv(src_task, src_trayIdx,
            dest_task, dest_trayIdx,
            count, MPIentityType,
            comm);
}


int OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){
	return _OMPI_XclWaitFor(numTasks, taskIds, comm);
}

int OMPI_XclWaitAllTasks(MPI_Comm comm){
	return _OMPI_XclWaitAllTasks(comm);
}
