
#include "dataCopy.h"

#define DEBUG 0
#define PROFILE 0

int writeBuffer(int taskId, int trayIdx, int bufferSize, void * hostBuffer){
	int status;
	int myRack=l_taskList[taskId].Rack;
	/*cl_event memProfileEvent;
	cl_ulong time_start, time_end;
	double total_time;*/
	//pthread_mutex_lock(&deviceQueueMutex);
	//cl_event writeDone;
		status = clEnqueueWriteBuffer(l_taskList[taskId].device[0].queue,
				l_taskList[taskId].device[0].memHandler[myRack][trayIdx], CL_TRUE, 0, bufferSize,
				hostBuffer, 0, NULL, NULL);
		clFlush(l_taskList[taskId].device[0].queue);
		clFinish(l_taskList[taskId].device[0].queue);
		/*clWaitForEvents(1, &writeDone);*/
	//pthread_mutex_unlock(&deviceQueueMutex);

		chkerr(status, "Writing the new mem Buffer", __FILE__, __LINE__);
		debug_print("--write done in task %d, rack %d, tray: %d. \n ",taskId,myRack,trayIdx);

#if PROFILE
		clWaitForEvents(1, &memProfileEvent);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_END,
				sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		profile_print("SINGLE buffer writing in task: %d  took -->  %0.3f ms\n", taskId,
				(total_time / 1000000.0));
#endif //if PROFILE



	return status;

}

/*PINNED DATA COPY.
 *
 * This type of copy is favorable for the CPU because is a 0 copy strategy.
 *


void **p;
p=(void**)malloc(numTasks*sizeof(void*));

for (i = 0; i < numTasks; i++) {
	p[i] = (void *) clEnqueueMapBuffer(l_taskList[i].device[0].queue,  //map memory object on device to memory in host.
			l_taskList[i].device[0].memHandler[0], CL_TRUE,
			CL_MAP_WRITE, 0, bufferSize,
			0, NULL, NULL, &status);

	//memset(p, 0, task->kernel[0].buffer[0].size*sizeof(cl_float3));
	p[i]=*entitiesbuffer;
	status |= clEnqueueUnmapMemObject(l_taskList[i].device[0].queue, l_taskList[i].device[0].memHandler[0], p[i], 0, NULL,NULL);
	chkerr(status, "error at Writing Buffers", __FILE__, __LINE__);
}
*/



