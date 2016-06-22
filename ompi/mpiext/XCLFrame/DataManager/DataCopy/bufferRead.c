#include "dataCopy.h"
#define DEBUG 0

int readBuffer(int taskId,int trayIdx, int bufferSize, void * hostBuffer){
	int status;
	int myRack=l_taskList[taskId].Rack;

	debug_print("No. l_Task: %d, No. Rack= %d \n",taskId,myRack);
pthread_mutex_lock(&deviceQueueMutex);
	cl_event readDone;
	status = clEnqueueReadBuffer(l_taskList[taskId].device[0].queue,
					l_taskList[taskId].device[0].memHandler[myRack][trayIdx], CL_TRUE, 0, bufferSize,
					hostBuffer, 0, NULL, &readDone);
	clFlush(l_taskList[taskId].device[0].queue);
	clFinish(l_taskList[taskId].device[0].queue);
	clWaitForEvents(1, &readDone);
pthread_mutex_unlock(&deviceQueueMutex);

		//printf("\n-->taskId: %d memIdx: %d bufSize:%d, vs %d  \n",taskId ,memIdx, bufferSize,DbufferSize[0]);
		chkerr(status, "Reading mem Buffers", __FILE__, __LINE__);
		//clFinish(l_taskList[taskId].device[0].queue);
	return status;
}


