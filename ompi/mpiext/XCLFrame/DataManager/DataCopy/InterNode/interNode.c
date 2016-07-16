
#include "distLinkedList.h"
#include "mcs-mutex.h"
#include "../../Base/dataManager.h"


pthread_mutex_t dListAccess=PTHREAD_MUTEX_INITIALIZER;

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

	//grant Unique Access against other threads.
	int success=0;

	do{
		pthread_mutex_lock(&dListAccess);
		success=find_if(tgID); //only one thread access the distributed list (working like a single process)
		pthread_mutex_unlock(&dListAccess);
	}while(!success);

	//here the receiver is ready =)
	    pthread_mutex_lock(&dListAccess);
		MCS_Mutex_lock(global_mtx);
        MPI_Accumulate(&tgID, 1, MPI_INT,
        			  myRank, MCS_WIN_TAG, 1, MPI_INT, MPI_REPLACE, global_mtx->window);
        MPI_Win_flush(myRank, global_mtx->window);

        //NOW A TYPICAL SEND

    	void* tmpBuffData;
    	MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO:  this might conflict with scatter measure.
    	int tmpBuffSz = count * MPIentityTypeSize;
    	tmpBuffData = (void*) malloc(tmpBuffSz);

    	readBuffer(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);
    	int dest_rank = g_taskList[g_dest_task].r_rank;
    	int ack;
    	MPI_Request request;
    	MPI_Status status;
    	int flag=0;
    	MPI_Issend(tmpBuffData, count, MPIentityType, dest_rank, tgID, comm, &request);
    	while (!flag)
    	{
    		MPI_Test(&request, &flag, &status);
    	}
		MCS_Mutex_unlock(global_mtx);
		pthread_mutex_unlock(&dListAccess);

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
	int tgID			 =recv_Args->tgID;
	MPI_Comm comm	 =recv_Args->comm;


	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int src_rank = g_taskList[g_src_task].r_rank;

	pthread_mutex_lock(&dListAccess);
		dstrContLst_push(tgID);
	pthread_mutex_unlock(&dListAccess);

	for(;;){
		int value =-1;
		MPI_Fetch_and_op(NULL, &value, MPI_INT, src_rank, MCS_WIN_TAG, MPI_NO_OP,
				global_mtx->window);
		MPI_Win_flush(global_mtx->home_rank, global_mtx->window);
		if(value==tgID) break;
	}


		void* tmpBuffData;
		MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO: this might conflict with scatter measure.
		int tmpBuffSz = count * MPIentityTypeSize; //buffer size in bytes.
		tmpBuffData = (void*) malloc(tmpBuffSz);

		MPI_Request request;
		MPI_Status status;
		int flag=0;
		MPI_Irecv(tmpBuffData, count, MPIentityType, src_rank, tgID, comm,&request);
		MPI_Test(&request, &flag, &status);
		while (!flag)
		{
			MPI_Test(&request, &flag, &status);
		}

		//initNewBuffer(l_recv_task, trayIdx, tmpBuffSz);
		writeBuffer(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);


	return 0;
}
