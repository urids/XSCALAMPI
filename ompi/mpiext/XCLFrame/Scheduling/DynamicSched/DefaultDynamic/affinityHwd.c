#include "dynamicSched.h"

int affinInitNewTask (task_t** task, int numTasks, int Rank, int DeviceType, int DeviceID, MPI_Comm comm){
	insertThreads(1, 0);
	taskThreadsEnabled=1;
	////WARNINNG FIXED ASSIGNMENT IS WRONG!!!
	//reallocLocalTaskList(CPU_DEVICES,0);
	//reallocGlobalTaskList(Rank);
	return 0;
}



