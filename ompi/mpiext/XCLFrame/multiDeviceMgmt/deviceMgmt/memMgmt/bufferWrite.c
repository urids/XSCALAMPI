
#include "bufferFunctions.h"
#define DEBUG 0
#define PROFILE 0

int writeBuffer(int taskId, int trayIdx, int bufferSize, void * hostBuffer){
	int status;
	int myRackID=l_taskList[taskId].RackID;
	cl_event memProfileEvent;
	cl_ulong time_start, time_end;
	double total_time;

		status = clEnqueueWriteBuffer(l_taskList[taskId].device[0].queue,
				l_taskList[taskId].device[0].memHandler[myRackID][trayIdx], CL_TRUE, 0, bufferSize,
				hostBuffer, 0, NULL, &memProfileEvent);
		//status=clFlush(l_taskList[taskId].device[0].queue);

		chkerr(status, "Writing the new mem Buffer", __FILE__, __LINE__);
		debug_print("--write done in task %d, rack %d, tray: %d. \n ",taskId,myRackID,trayIdx);

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


int ALL_TASKS_writeBuffer(int numTasks, int trayIdx, void * hostBuffer ) {
	int i,j; //index variables
	int status=0;

	void* tmpBuffer=hostBuffer;
	for (i = 0; i < numTasks; i++) {
		cl_event memProfileEvent;
		cl_ulong time_start, time_end;
		double total_time;

		int myRackID=l_taskList[i].RackID;
				status = clEnqueueWriteBuffer(l_taskList[i].device[0].queue,
								l_taskList[i].device[0].memHandler[myRackID][trayIdx], CL_TRUE,0,
								tskbufferSize[i], //extern DbuffersSize is declared in bufferFunctions.
								tmpBuffer,
								0, NULL, &memProfileEvent);
				tmpBuffer+=tskbufferSize[0]; //each task writing must do an offset.
			chkerr(status, "Writing mem Buffers", __FILE__, __LINE__);
			debug_print("copied: %zd \n" ,l_taskList[i].trayInfo->size);



#if PROFILE
		clWaitForEvents(1, &memProfileEvent);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_END,
				sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		profile_print("SCATTER buffer writing in task: %d  took -->  %0.3f ms\n", i,
				(total_time / 1000000.0));
#endif //if PROFILE



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


		return status;
}



