#include "bufferFunctions.h"
#define DEBUG 0

void XclFreeTaskBuffer(int l_taskIdx, int trayIdx){

	int myRack=taskList[l_taskIdx].Rack;
	cl_int status;
	status=clReleaseMemObject (taskList[l_taskIdx].device[0].memHandler[myRack][trayIdx]);
	chkerr(status, "Error at: Freeing Mem Buffer, %s: %d", __FILE__, __LINE__);

}
