#include "bufferFunctions.h"
#define DEBUG 0
#define PROFILE 0
				/*WARNINIG!!!!!!!!!*/
int intracpyBuffer(int src_taskId, int srcTrayIdx,int dst_taskId, int dstTrayIdx, int bufferSize){ //TODO: by now I'm assuming both buffers exists =P
	int status;
		int srcRack=taskList[src_taskId].Rack;
		int dstRack=taskList[dst_taskId].Rack;

		debug_print("intra-device Copy stage 1 in task %d \n",src_taskId);
		cl_mem srcTray=taskList[src_taskId].device[0].memHandler[srcRack][srcTrayIdx];
		cl_mem dstTray=taskList[dst_taskId].device[0].memHandler[dstRack][dstTrayIdx];

		status=clEnqueueCopyBuffer(taskList[src_taskId].device[0].queue ,srcTray,dstTray,0,0,bufferSize,0,NULL,NULL);
		clFlush(taskList[src_taskId].device[0].queue);
		clFinish(taskList[src_taskId].device[0].queue); //enforces synchronization =)
		chkerr(status, "Intra device mem cpy Buffer", __FILE__, __LINE__);
		debug_print("--write done in task %d, rack %d, tray: %d. \n ",src_taskId,srcRack,dstTray);

		return status;
}
