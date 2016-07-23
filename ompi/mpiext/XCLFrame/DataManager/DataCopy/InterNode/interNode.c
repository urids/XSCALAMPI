
#include "distLinkedList.h"
#include "mcs-mutex.h"
#include "../../Base/dataManager.h"


pthread_mutex_t dListAccess=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t senderMutex=PTHREAD_MUTEX_INITIALIZER;

int interNodeCpyProducer(void* Args){
	//1.- unwrap the Args.
	struct Args_Send_st * send_Args=(struct Args_Send_st*) Args;
	int trayIdx 	 =send_Args->trayIdx;
	int count		 =send_Args->count;
	MPI_Datatype MPIentityType=send_Args->MPIentityType;
	int l_src_task   =send_Args->l_src_task;
	int g_dest_task  =send_Args->g_dest_task;
	int tgID		 =send_Args->tgID;
	MPI_Comm comm	 =send_Args->comm;

	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int dest_rank = g_taskList[g_dest_task].r_rank;
	//grant Unique Access against other threads.
	/*int success=0;
	do{
		pthread_mutex_lock(&dListAccess);
		success=find_if(tgID); //only one thread access the distributed list (working like a single process)
		pthread_mutex_unlock(&dListAccess);
	}while(!success);
//printf("(Process= %d):: receiver found \n",myRank);

	//here the receiver is ready =)
	    pthread_mutex_lock(&dListAccess);
		MCS_Mutex_lock(global_mtx);

		int value =-1;

        MPI_Accumulate(&tgID, 1, MPI_INT,
        			  myRank, MCS_WIN_TAG, 1, MPI_INT, MPI_REPLACE, global_mtx->window);
        MPI_Win_flush(myRank, global_mtx->window);
    	MPI_Win_sync(global_mtx->window);

    	//MPI_Fetch_and_op(NULL, &value, MPI_INT, myRank, MCS_WIN_TAG, MPI_NO_OP,
        				//global_mtx->window);
        //printf("I'm process: %d, after val:= %d \n",myRank,value);

        */
        //NOW A TYPICAL SEND

    	void* tmpBuffData;
    	MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO:  this might conflict with scatter measure.
    	int tmpBuffSz = count * MPIentityTypeSize;
    	tmpBuffData = (void*) malloc(tmpBuffSz);

    	readBuffer(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);
    	MPI_Send(tmpBuffData,count,MPIentityType,dest_rank,tgID,comm);

    	//MPI_Request request;
    	//MPI_Status status;
    	//int flag=0;
    	//MPI_Issend(tmpBuffData, count, MPIentityType, dest_rank, tgID, comm, &request);
    	//while (!flag)
    	//{
    		//MPI_Test(&request, &flag, &status);
    	//}




/*---		int value=-1;
		int value2,i;

		for(;;){
			pthread_mutex_lock(&senderMutex);

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dest_rank, 0, interNode_win);
			MPI_Win_sync(interNode_win);
			//MPI_Get(&value,1,MPI_INT,dest_rank,g_dest_task,1,MPI_INT,interNode_win);
			MPI_Fetch_and_op(NULL,&value,MPI_INT,dest_rank,g_dest_task,MPI_NO_OP,interNode_win);
			MPI_Win_flush(dest_rank, interNode_win);
			MPI_Win_unlock(dest_rank, interNode_win);


			//DEBUG Purposes.
		 /* for(i=0;i<g_numTasks;i++){
		 		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, dest_rank, 0, interNode_win);
				MPI_Win_sync(interNode_win);
				MPI_Fetch_and_op(NULL,&value2,MPI_INT,dest_rank,i,MPI_NO_OP,interNode_win);
				MPI_Win_flush(dest_rank, interNode_win);
				MPI_Win_unlock(dest_rank, interNode_win);
				printf("%d -- ",value2);
			}*/

/*---			printf("TAG: %d in g_detsTask: %d \n", value,g_dest_task);
			if(value==tgID){
				MPI_Send(tmpBuffData,count,MPIentityType,dest_rank,tgID,comm);
				pthread_mutex_unlock(&senderMutex);
				break;
			}
			pthread_mutex_unlock(&senderMutex);
		}



		//pthread_mutex_lock(&senderMutex);

		//pthread_mutex_unlock(&senderMutex);

    	//MCS_Mutex_unlock(global_mtx);

		//printf("(Process= %d):: send done. \n",myRank);*/
	return 0;
}

int interNodeCpyConsumer(void* Args){
	//1.- unwrap the Args.
	struct Args_Recv_st * recv_Args=(struct Args_Recv_st*)Args;

	int trayIdx		 =recv_Args->trayIdx;
	int count		 =recv_Args->count;
	MPI_Datatype MPIentityType = recv_Args->MPIentityType;
	int g_src_task   =recv_Args->g_src_task;
	int l_recv_task  =recv_Args->l_recv_task;
	int tgID		 =recv_Args->tgID;
	MPI_Comm comm	 =recv_Args->comm;

	int i;
	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int src_rank = g_taskList[g_src_task].r_rank;
	int g_recv_task=0;
	//for(i=0;i<g_numTasks;i++)
	//printf(" %d - %d - %d \n",g_taskList[i].g_taskIdx,g_taskList[i].l_taskIdx,g_taskList[i].r_rank);

	/*while(g_taskList[g_recv_task].l_taskIdx!=l_recv_task || g_taskList[g_recv_task].r_rank!=myRank){
		g_recv_task++;
	}*/

	//printf("global recv: %d",g_recv_task);


/*
	pthread_mutex_lock(&dListAccess);
		dstrContLst_push(tgID);
	pthread_mutex_unlock(&dListAccess);

	for(;;){
		int value =-1;
		//int value2 =-1;
		pthread_mutex_lock(&dListAccess);

		MPI_Win_flush(src_rank, global_mtx->window);
		MPI_Fetch_and_op(NULL, &value, MPI_INT, src_rank, MCS_WIN_TAG, MPI_NO_OP,
				global_mtx->window);
		MPI_Win_sync(global_mtx->window);
		pthread_mutex_unlock(&dListAccess);
		//printf("I'm process: %d, srcRank= %d, curr val:= %d \n",myRank,src_rank,value);

		//MPI_Get(&value2,1,MPI_INT,1,2,1,MPI_INT,global_mtx->window);
		//printf("I'm process: %d, val2:= %d \n",myRank,value2);
		if(value==tgID) break;
	}
*/

		//initNewBuffer(l_recv_task, trayIdx, tmpBuffSz);

	//pthread_mutex_unlock(&dListAccess);
	//printf("(Process= %d):: receipt complete! \n",myRank);



/*---		pthread_mutex_lock(&dListAccess);
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, myRank, 0, interNode_win);
		//MPI_Win_sync(interNode_win);
		//MPI_Put(&tgID,1,MPI_INT,myRank,g_recv_task,1,MPI_INT,interNode_win);
		MPI_Accumulate(&tgID,1,MPI_INT,myRank,g_recv_task,1,MPI_INT,MPI_REPLACE,interNode_win);
		MPI_Win_flush(myRank, interNode_win);
		MPI_Win_unlock(myRank, interNode_win);

	/*	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, myRank, 0, interNode_win);

		MPI_Get(&gvalue,1,MPI_INT,myRank,g_recv_task,1,MPI_INT,interNode_win);

		MPI_Win_unlock(myRank, interNode_win);*/


//MPI_Win_flush(myRank, interNode_win);
	//	MPI_Fetch_and_op(NULL, &gvalue, MPI_INT, myRank, 0, MPI_NO_OP,
		//				interNode_win);


		void* tmpBuffData;
		MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO: this might conflict with scatter measure.
		int tmpBuffSz = count * MPIentityTypeSize; //buffer size in bytes.
		tmpBuffData = (void*) malloc(tmpBuffSz);



		MPI_Recv(tmpBuffData,count,MPIentityType,src_rank,tgID,comm,NULL);
		writeBuffer(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);

		//pthread_mutex_unlock(&dListAccess);
		/*MPI_Request request;
		MPI_Status status;
		int flag=0;
		MPI_Irecv(tmpBuffData, count, MPIentityType, src_rank, tgID, comm,&request);
		MPI_Test(&request, &flag, &status);
		while (!flag)
		{
			MPI_Test(&request, &flag, &status);
		}*/






		return 0;
}
