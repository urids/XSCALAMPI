#include "bufferFunctions.h"
#define DEBUG 0

int numEntitiesInHost;
float entitiesPerTask;
int* tskbufferSize; //Device BufferSize global variable declared in bufferFunctions.h

/* *
 * Buffer initialization has two stages:
 * 1.- RackID initialization (DONE in tskMgmt.c at createTaskList() )
 * 2.- Trays initialization.
 * */

int initNewBuffer(int taskIdx, int trayIdx, int bufferSize ){

	cl_mem * tmpMemHandler=NULL;
	int status;

	int memIdx=l_taskList[taskIdx].numTrays; //here we query how many buffers has this task
	int myRackID=l_taskList[taskIdx].RackID;
	debug_print("initializing ::: task: %d rack: %d tray: %d :::\n",taskIdx,myRackID,trayIdx);

	//if (trayIdx==0 && memIdx==0 ){ //if this is the first tray to be initialized.
	debug_print("task %d has -%d- trays \n",taskIdx,memIdx);
	if (memIdx==0){//TODO: WARNING!!!!! memindx is cero on rack? <- This is BUG DETECTED
			l_taskList[taskIdx].trayInfo = malloc(sizeof(XCLTrayInfo));
			l_taskList[taskIdx].device[0].memHandler[myRackID] = malloc(trayIdx+1 * sizeof(cl_mem)); //init the first device mem space.
			l_taskList[taskIdx].numTrays=trayIdx+1;

			int j=0;
			for(j=0;j<trayIdx+1;j++){
			l_taskList[taskIdx].device[0].memHandler[myRackID][j] = clCreateBuffer(
						l_taskList[taskIdx].device[0].context,
						CL_MEM_READ_WRITE,
						bufferSize,
						NULL,
						&status);
			chkerr(status, "Error at: Creating new Mem Buffer, %s: %d", __FILE__, __LINE__);
			}

		}

	else if ( (trayIdx+1) > memIdx ) { //strictly great to avoid reallocations
		cl_mem * tmpMemHandler;
		tmpMemHandler = realloc(l_taskList[taskIdx].device[0].memHandler[myRackID],
				(trayIdx + 1) * sizeof(cl_mem)); //(tray + 1) to avoid dereferencing issues.

		if (tmpMemHandler != NULL){
			l_taskList[taskIdx].device[0].memHandler[myRackID] = tmpMemHandler;
			l_taskList[taskIdx].numTrays=(memIdx+1); //here we inform to the runtime that this device has increased its number of memObjs
			debug_print("init DONE ::: task: %d rack: %d tray: %d :::\n",taskIdx,myRackID,trayIdx);
		}else{
			free(l_taskList[taskIdx].device[0].memHandler[myRackID]);
			printf("Error AT:(re)allocating memory %s ,%s",__FILE__,__LINE__);
			exit(1);
		}
		//free(tmpMemHandler); TODO: is it now ready to free?

		l_taskList[taskIdx].device[0].memHandler[myRackID][trayIdx] = clCreateBuffer(
								l_taskList[taskIdx].device[0].context,
								CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, //best practice
								bufferSize,
								NULL,
								&status);

		/*This section ensures proper device memory allocation.*/
		char * dset=malloc(3*sizeof(char));//="0\0";
	/*	dset=clEnqueueMapBuffer(l_taskList[taskIdx].device[0].queue,
								l_taskList[taskIdx].device[0].memHandler[myRackID][trayIdx],
								CL_TRUE,
								CL_MAP_WRITE,
								bufferSize-2,
								bufferSize,
								0,NULL,NULL,NULL);*/
		dset="a\0";
		status|=clEnqueueWriteBuffer(l_taskList[taskIdx].device[0].queue,
								  l_taskList[taskIdx].device[0].memHandler[myRackID][trayIdx],
								  CL_TRUE,
								  bufferSize-2,
								  2,
								  dset,
								  0,NULL,NULL);

/*
		status|=clEnqueueUnmapMemObject(l_taskList[taskIdx].device[0].queue,
								 l_taskList[taskIdx].device[0].memHandler[myRackID][trayIdx],
								  (void*)dset,
								  0,NULL,NULL);
*/

		chkerr(status, "Error at: Creating new Mem Buffer", __FILE__, __LINE__);

	}

	//TODO: missing trayInfo space allocation
    //l_taskList[taskIdx].trayInfo->size = bufferSize;
	//l_taskList[taskIdx].trayInfo->bufferMode = CL_MEM_READ_WRITE;

/*	l_taskList[taskIdx].device[0].memHandler[myRackID][trayIdx] = clCreateBuffer(
			l_taskList[taskIdx].device[0].context,
			CL_MEM_READ_WRITE,
			bufferSize,
			NULL,
			&status);

			chkerr(status, "Error at: Creating new Mem Buffer, %s: %d", __FILE__, __LINE__);
*/



return status;

}

// HbufferSize &  MPIentityTypeSize enables determine the hostBuffer offset for each task
// Hbuffer is the complete buffer residing in this rank.
int  ALL_TASKS_initBuffer(int numTasks,int trayIdx, int HbufferSize, int MPIentityTypeSize) {
	int status=0;
	int i,j;//index variables

	//if(l_taskList[i].device[0].memHandler==NULL)
		//l_taskList[i].device[0].memHandler = (cl_mem**)malloc(numTasks * sizeof(cl_mem*));

	numEntitiesInHost = (int) HbufferSize / MPIentityTypeSize;
	entitiesPerTask = (float) numEntitiesInHost / (float) numTasks;
	tskbufferSize = (int*) malloc(numTasks * sizeof(int)); //Saves the Device BufferSize



/*
	for (i = 0; i < numTasks; i++) {
		if(DbufferSize<=l_taskList[i].device->maxAllocMemSize){
			printf("enough Space =) %d in Host  %d Max Alloc in Device %d Max Device Space \n"
				 ,HbufferSize,l_taskList[i].device->maxAllocMemSize,
				 l_taskList[i].device->globalMemSize);
		}
	}
*/

	for(j=0;j<numTasks;j++){ //TODO: make this more readable: This instruction sets the size of the device memory buffer for each task.
		(j==(numTasks-1))?(tskbufferSize[j]=(int)floorf(entitiesPerTask)*MPIentityTypeSize):(tskbufferSize[j]=(int)ceilf(entitiesPerTask)*MPIentityTypeSize);
	}


	for (i = 0; i < numTasks; i++) {
		int memIdx = l_taskList[i].numTrays; //here we query how many buffers exist in this device
		int myRackID=l_taskList[i].RackID;
		if (memIdx < trayIdx){ //strictly less to avoid reallocations
			cl_mem * tmpMemHandler;
			tmpMemHandler = realloc(l_taskList[i].device[0].memHandler[myRackID],
					(trayIdx+1) * sizeof(cl_mem)); //(tray + 1) to avoid dereferencing issues.
			if (tmpMemHandler != NULL){
				l_taskList[i].device[0].memHandler[myRackID] = tmpMemHandler;
				l_taskList[i].numTrays=(trayIdx+1);
			}
			else {
				free(l_taskList[i].device[0].memHandler[myRackID]);
				printf("Error AT:(re)allocating memory %s:%d",__FILE__,__LINE__);
				exit(1);
			}
			//free(tmpMemHandler); //TODO: is it now ready to free?
		}
		 if (trayIdx==0 && memIdx==0){ //if this is the first tray to be initialized.
			l_taskList[i].trayInfo = malloc(sizeof(XCLTrayInfo));
			l_taskList[i].device[0].memHandler[myRackID] = malloc(1 * sizeof(cl_mem)); //init the first device mem space.
			l_taskList[i].numTrays=1;
		}

		l_taskList[i].trayInfo->size = tskbufferSize[i];//device buffer size for the i-th task.
		//l_taskList[i].entitiesbuffer->bufferMode = CL_MEM_READ_WRITE; //TODO review this issue

		l_taskList[i].device[0].memHandler[myRackID][trayIdx] = clCreateBuffer(
				l_taskList[i].device[0].context,
				CL_MEM_READ_WRITE,
				l_taskList[i].trayInfo->size, NULL, &status);

		chkerr(status, "Error at: Creating mem Buffers", __FILE__, __LINE__);
		debug_print("allocated: %zd Bytes in Device Memory\n ", l_taskList[i].trayInfo->size);

	}

	return status;
}
