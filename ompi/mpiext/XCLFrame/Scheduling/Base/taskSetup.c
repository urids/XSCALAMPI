
#include "scheduling.h"

int fillGlobalTaskList(MPI_Comm comm){
	int i,j;
	int myRank,numRanks;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm, &numRanks);

	g_taskList=(taskInfo*)malloc(g_numTasks*sizeof(taskInfo)); //TODO: find the space for deallocation.

	int* RKS =(int*)malloc(numRanks*sizeof(int)); //RKS-> RanK Structure
	MPI_Allgather(&l_numTasks,1,MPI_INT,RKS,1,MPI_INT,comm);

		for(i=0;i<g_numTasks;i++){
			g_taskList[i].g_taskIdx=taskDevList[i].g_taskId;
			g_taskList[i].r_rank=taskDevList[i].rank;
		}

		int k;
		for(i=0,k=0;i<numRanks;i++){
			for(j=0;j<RKS[i];j++,k++){
				g_taskList[k].l_taskIdx=j;
			}
		}

		printf("in %d, %d, %d",__FILE__,__FUNCTION__,__LINE__);
		for(i=0;i<g_numTasks;i++){ //Print the global task list
			printf("%d -- %d -- %d \n",g_taskList[i].g_taskIdx, g_taskList[i].l_taskIdx, g_taskList[i].r_rank);
		}

	return 0;
}


int fillLocalTaskList(MPI_Comm comm){
	int i;
	int myRank,numRanks;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);

	//int l_numTasks=0; !!!!!!!!!!!!!!!!!!!NEVER INITIALIZE GLOBAL VARS INSIDE A FUNCTIONS
	//XCLtask * l_taskList=NULL; !!!!!!!THE FUNCTION WORKS WITH A LOCAL COPY ='(..

	for(i=0;i<g_numTasks;i++){ //EACH PROCESS: read the whole tasDevList created when parsing to fill his # of tasks
	int TDL_rank=taskDevList[i].rank;
		if(myRank==TDL_rank){
			int j=l_numTasks;
			l_taskList = (XCLtask*)realloc(l_taskList, (l_numTasks+1)*sizeof(XCLtask));
			l_taskList[j].device=(Device*) malloc(1*sizeof(Device));
			l_taskList[j].device = taskDevList[i].mappedDevice;
			l_taskList[j].numTrays = 0;

			//here we query how many racks has this device.
			int rackIdx = l_taskList[j].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[j].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				l_taskList[j].Rack = rackIdx; //rack assignment
				l_taskList[j].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[j].device[0].memHandler,(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[j].Rack = rackIdx;
					l_taskList[j].device[0].memHandler = tmpRack;
					l_taskList[j].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d",	__FILE__, __LINE__);
				}
			}

			l_numTasks++;
		}//ENDif myRank==TskDevList_rank

		MPI_Barrier(comm); //To ensure all processes have counted their tasks.
	}
	return 0;
}


int taskSetup(int devSelection){
int i,j;
	switch (devSelection) {
	case ALL_DEVICES:
		//here we allocate space for the local l_taskList.
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		//Each task needs a handler (pointer) to its device
		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(1*sizeof(Device)); //each task has only 1 device.
		}

		//int advance=0;
		//Here we fill the l_taskList and create the memory Racks.
		//for (i = 0; i <l_numTasks; i++) {
		for (i = 0; i <numDecls; i++) {
			//advance=taskDevMap[i].max_tskIdx-taskDevMap[i].min_tskIdx+1;
			if(taskDevMap[i].min_tskIdx!=(-1))//perform resource allocation iff has any assign.
			for (j = taskDevMap[i].min_tskIdx; j <= taskDevMap[i].max_tskIdx;j++) {
				debug_print("-----matching task %d , %d- %d ------\n",j,taskDevMap[i].min_tskIdx,taskDevMap[i].max_tskIdx);

				l_taskList[j].device = taskDevMap[i].mappedDevice;
				l_taskList[j].numTrays = 0;

				//here we query how many racks has this device.
				int rackIdx = l_taskList[j].device[0].numRacks;
				if (rackIdx == 0) {
					l_taskList[j].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
					//l_taskList[j].device[0].memHandler[0] = malloc(1 * sizeof(cl_mem));
					l_taskList[j].Rack = rackIdx; //rack assignment
					l_taskList[j].device[0].numRacks++;
				} else {
					cl_mem** tmpRack;
					tmpRack = (cl_mem**) realloc(l_taskList[j].device[0].memHandler,(rackIdx + 1) * sizeof(cl_mem*));
					if (tmpRack != NULL) {
						l_taskList[j].Rack = rackIdx;
						l_taskList[j].device[0].memHandler = tmpRack;
						l_taskList[j].device[0].numRacks++;
					} else {
						printf("ERROR AT: Reallocating racks. %d , %d",	__FILE__, __LINE__);
					}
				}
			}

		}

		break;



	case CPU_DEVICES:
		//l_numTasks = 4;
		printf("CPU EXEC\n");
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);


		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = cpu[0];//TODO: switch device number.
			l_taskList[i].numTrays = 0; //init the number of device memBuffers to zero

			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}

			//l_taskList[i].Rack=i;
			//l_taskList[i].device[0].numRacks++;
		}
		break;

	case GPU_DEVICES:
		//l_numTasks = 4;
		printf("GPU EXEC\n");
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = gpu[0]; //TODO: switch device number.
			l_taskList[i].numTrays = 0; //init the number of device memBuffers to zero

			//here we query how many racks has this device.
			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				//l_taskList[i].device[0].memHandler[0]=malloc(1*sizeof(cl_mem));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}
		}

		break;

	case ACC_DEVICES:
		//l_numTasks = clXplr.numACCEL;
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = accel[i];
			l_taskList[i].numTrays=0;//init the number of device memBuffers to zero
			//here we query how many racks has this device.
			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				//l_taskList[i].device[0].memHandler[0]=malloc(1*sizeof(cl_mem));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}

		}

		break;

	default:
		/*perror("Mismatched option please select: \n "
			   "0 - ALL DEVICES \n "
			   "1 - GPU DEVICES \n "
			   "2 - CPU DEVICES \n "
			   "3 - ACCEL DEVICES");
		exit(1);*/
		break;
	}
return 0;
}
