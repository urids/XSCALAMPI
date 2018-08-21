#include "bufferFunctions.h"
#define DEBUG 0


int readBuffer(int taskId,int trayIdx, int bufferSize, void * hostBuffer){
	int status;
	int myRackID=l_taskList[taskId].RackID;

	debug_print("No. l_Task: %d, No. RackID= %d \n",taskId,myRackID);
	status = clEnqueueReadBuffer(l_taskList[taskId].device[0].queue,
					l_taskList[taskId].device[0].memHandler[myRackID][trayIdx], CL_TRUE, 0, bufferSize,
					hostBuffer, 0, NULL, NULL);
		//printf("\n-->taskId: %d memIdx: %d bufSize:%d, vs %d  \n",taskId ,memIdx, bufferSize,DbufferSize[0]);
		chkerr(status, "Reading mem Buffers", __FILE__, __LINE__);
		//clFinish(l_taskList[taskId].device[0].queue);
	return status;
}

int ALL_TASKS_readBuffer(int numTasks, int trayIdx ,void * hostBuffer) {
	int i,j;
	int status=0;


	void* tmpBuffer = hostBuffer;
	for (i = 0; i < numTasks; i++) {
		int myRackID=l_taskList[i].RackID;
		debug_print("read Request: %d \n" ,tskbufferSize[i]);
		debug_print("tmpBuffer %d \n" ,tmpBuffer);
		status = clEnqueueReadBuffer(l_taskList[i].device[0].queue,
				l_taskList[i].device[0].memHandler[myRackID][trayIdx], CL_TRUE, 0, tskbufferSize[i],
				tmpBuffer, 0, NULL, NULL);
		tmpBuffer+=tskbufferSize[0];
		chkerr(status, "Reading mem Buffers", __FILE__, __LINE__);
	}

	return status;
}
