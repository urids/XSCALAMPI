#include "bufferFunctions.h"
#define DEBUG 0
#define PROFILE 0
				/*WARNINIG!!!!!!!!!*/
int intracpyBuffer(int src_taskId, int srcTrayIdx,int dst_taskId, int dstTrayIdx, int bufferSize){ //TODO: by now I'm assuming both buffers exists =P
	int status;
		int srcRackID=l_taskList[src_taskId].RackID;
		int dstRackID=l_taskList[dst_taskId].RackID;

		debug_print("intra-device Copy stage 1 in task %d \n",src_taskId);
		cl_mem srcTray=l_taskList[src_taskId].device[0].memHandler[srcRackID][srcTrayIdx];
		cl_mem dstTray=l_taskList[dst_taskId].device[0].memHandler[dstRackID][dstTrayIdx];

		status=clEnqueueCopyBuffer(l_taskList[src_taskId].device[0].queue ,srcTray,dstTray,0,0,bufferSize,0,NULL,NULL);
		clFlush(l_taskList[src_taskId].device[0].queue);
		clFinish(l_taskList[src_taskId].device[0].queue); //enforces synchronization =)
		chkerr(status, "Intra device mem cpy Buffer", __FILE__, __LINE__);
		debug_print("--write done in task %d, rack %d, tray: %d. \n ",src_taskId,srcRackID,dstTray);

		return status;
}
