#include "bufferFunctions.h"
#define DEBUG 0

void XclFreeTaskBuffer(int l_taskIdx, int trayIdx){

	int myRackID=l_taskList[l_taskIdx].RackID;
	cl_int status;
	status=clReleaseMemObject (l_taskList[l_taskIdx].device[0].memHandler[myRackID][trayIdx]);
	chkerr(status, "Error at: Freeing Mem Buffer, %s: %d", __FILE__, __LINE__);

}
