
#include "callsBag.h"
static int currNumTasks=0;
queue* bagSubRoutinesQueues;


void initNewBag(int numTasks){
	bagSubRoutinesQueues=malloc(numTasks*sizeof(queue));
	for(i=0;i<numTasks;i++){
		bagSubRoutinesQueues[i]=newQueue();
	}
	currNumTasks+=numTasks;
}

void reallocBag(int numTasks){
	int i;
	int newNumThds=currNumTasks+numTasks;

	bagSubRoutinesQueues=realloc(bagSubRoutinesQueues,newNumThds*sizeof(queue));
	for(i=currNumTasks;i<newNumThds;i++){
		bagSubRoutinesQueues[i]=newQueue();
	}
	currNumTasks+=numTasks;
}

int storeSubRoutine(int taskID, void*(*fp)(void*) ,void* Args){
		push(&bagSubRoutinesQueues[taskID],fp,Args);
	return 0;
}

void delegateSubRoutines(int g_taskID, int copyMode){
	printf("DELEGATION \n");
	//if (myRank == g_taskList[g_taskID].r_rank)
	int l_TaskID= g_taskList[g_taskID].l_taskIdx;
	node_t* subRtQueue_Node=malloc(sizeof(node_t));

	//subRtQueue_Node=bagSubRoutinesQueues[g_taskID];
	//if(bagSubRoutinesQueues[g_taskID]!=NULL) printf("first\n");

	while(bagSubRoutinesQueues[g_taskID]!=NULL){
		if(subRtQueue_Node->function==&_intraDevCpyProducer){
			//lets find out the destiny task
						struct Args_Send_st * snd_Args;
						int g_dstTask;
						switch (copyMode) {
							case INTRADEVICE:
								g_dstTask=((struct Args_matchedProducer_st *)(subRtQueue_Node->Args))->g_dstTaskID;
								break;
							case INTERDEVICE:
								g_dstTask=((struct Args_matchedProducer_st *)(subRtQueue_Node->Args))->g_dstTaskID;
								break;
							case INTERNODE:
								g_dstTask=((struct Args_Send_st *)(subRtQueue_Node->Args))->g_dest_task;
								break;
							default:
								break;
						}
						printf("glt: %d \n",g_taskList[g_dstTask]);
						if (taskDevList[i].mappedDevice!=NULL){ //dest task is assigned to some device
							//if (myRank == g_taskList[g_taskID].r_rank)
							pop(&bagSubRoutinesQueues[g_taskID],&subRtQueue_Node);//retrieve the subroutine
							addSubRoutine(l_TaskID, subRtQueue_Node->function ,subRtQueue_Node->Args);
						}
						else{
							break;
						}
		}else{
			printf("l_TaskID: %d \n",l_TaskID);
			//if (myRank == g_taskList[g_taskID].r_rank)
			pop(&bagSubRoutinesQueues[g_taskID],&subRtQueue_Node);//retrieve the subroutine
			//::DATA INTEGRATION::
			if(subRtQueue_Node->function==_OMPI_XclSetProcedure){
				((struct Args_SetProcedure_st *)(subRtQueue_Node->Args))->l_selTask=l_TaskID;
			}

			addSubRoutine(l_TaskID, subRtQueue_Node->function ,subRtQueue_Node->Args);
		}
	}
}


