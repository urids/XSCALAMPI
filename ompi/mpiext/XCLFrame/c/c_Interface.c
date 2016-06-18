

#include "c_Interface.h"

/* ==================================
 * | INIT OF GLOBAL DEFINITIONS |
 * ==================================
 * */

PUInfo* g_PUList; //Global Variable declared at localDevices.h
int g_numDevs;  //Global Variable declared at localDevices.h

taskInfo* g_taskList; //Global Variable declared at taskManager.h
int g_numTasks; //Global Variable declared at taskManager.h

struct timeval tval_globalInit; // Global variable declared in taskManager.h

pthread_mutex_t deviceQueueMutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t commMutex=PTHREAD_MUTEX_INITIALIZER;

list List;
sem_t FULL;
ticket_t opTicket;

/* ==================================
 * | END OF GLOBAL DEFINITIONS  |
 * ==================================
 * */
int MPIentityTypeSize;
int NON_DELEGATED OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype){
	return _OMPI_commit_EntityType(blockcount, blocklen,  displacements, basictypes, newDatatype);
}



int NON_DELEGATED OMPI_CollectDevicesInfo(int devSelection, MPI_Comm comm){
	return _OMPI_CollectDevicesInfo(devSelection, comm);
}


int NON_DELEGATED OMPI_CollectTaskInfo(int devSelection, MPI_Comm comm){

	/*	int numRanks, myRank;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);


	double off_secs,R_secs;
	double off_micrs,R_micrs;
	if(myRank==0){
		R_secs = (double) tval_result.tv_sec;
		R_micrs = (double) (tval_result.tv_usec);
	}

	MPI_Bcast(&R_secs,1,MPI_DOUBLE,0,comm);
	MPI_Bcast(&R_micrs,1,MPI_DOUBLE,0,comm);*/

	gettimeofday(&tval_globalInit, NULL);//TODO:  find the appropriate place to init the clock =0
	return _OMPI_CollectTaskInfo(devSelection, comm);
}


int OMPI_XclSetProcedure(MPI_Comm comm, int g_selTask, char* srcPath, char* kernelName){
	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank); //TODO: must be comm instead of MPI_COMM_WORLD
	if (myRank == g_taskList[g_selTask].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_selTask= g_taskList[g_selTask].l_taskIdx;
		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_SetProcedure_st * setProcedure_Args=malloc(sizeof(struct Args_SetProcedure_st));

		setProcedure_Args->l_selTask = l_selTask;
		setProcedure_Args->srcPath=srcPath;
		setProcedure_Args->kernelName=kernelName;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_selTask, _OMPI_XclSetProcedure, (void*)setProcedure_Args);

	}
	return 0; //return if the delegation succeed

	//return _OMPI_XclSetProcedure(comm, g_selTask, srcPath, kernelName);
}


int OMPI_XclExecTask(MPI_Comm communicator, int g_selTask, int workDim, size_t * globalThreads,
		size_t * localThreads, const char * fmt, ...) {

	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(communicator, &myRank);
	if (myRank == g_taskList[g_selTask].r_rank) {

		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_selTask= g_taskList[g_selTask].l_taskIdx;

		debug_print("local task %d requested in rank %d \n",l_selTask,myRank);

		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_ExecTask_st * execTask_Args=malloc(sizeof(struct Args_ExecTask_st));

		va_list argptr;
		va_start(argptr, fmt);
		va_end(argptr);

		execTask_Args->comm=communicator;
		execTask_Args->l_selTask=l_selTask;
		execTask_Args->workDim=workDim;
		execTask_Args->globalThreads=globalThreads;
		execTask_Args->localThreads=localThreads;
		execTask_Args->fmt=fmt;
		va_copy(execTask_Args->argsList,argptr);
		va_end(execTask_Args->argsList);
		//execTask_Args->argsList=argptr;

		char* profileFlag;
		int profilingEnabled = 0;
		profileFlag = getenv("XSCALA_PROFILING_APP"); //TODO: this profiling file might not be appropriated.
		if (profileFlag != NULL) {
			profilingEnabled = 1;
		}

		if (profilingEnabled) {
			//4.- Delegate the call to the thread.
			addSubRoutine(l_selTask, _OMPI_P_XclExecTask, (void*)execTask_Args);

			//_OMPI_P_XclExecTask(communicator, g_selTask, workDim, globalThreads,
			//	localThreads, tval_globalInit ,fmt, argptr);
		}else{
			//4.- Delegate the call to the thread.
			addSubRoutine(l_selTask, _OMPI_XclExecTask, (void*)execTask_Args);
			//_OMPI_XclExecTask(communicator, g_selTask, workDim, globalThreads,
			//	localThreads, fmt, argptr);
		}

		//va_end(argptr);

	}

	return 0;
}


int OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_WriteTray_st * writeTray_Args=malloc(sizeof(struct Args_WriteTray_st));
		writeTray_Args->l_taskIdx=l_taskIdx;
		writeTray_Args->trayIdx=trayIdx;
		writeTray_Args->bufferSize=bufferSize;
		writeTray_Args->hostBuffer=hostBuffer;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_taskIdx, _OMPI_XclWriteTray, (void*)writeTray_Args);

	}

	return 0; //return when the delegation succeed
	//return _OMPI_XclWriteTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
}

int OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm){
	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_ReadTray_st * readTray_Args=malloc(sizeof(struct Args_ReadTray_st));
		readTray_Args->l_taskIdx=l_taskIdx;
		readTray_Args->trayIdx=trayIdx;
		readTray_Args->bufferSize=bufferSize;
		readTray_Args->hostBuffer=hostBuffer;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_taskIdx, _OMPI_XclReadTray, (void*)readTray_Args);
	}

	//return _OMPI_XclReadTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
	return 0;
}

int OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){

	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;

		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_MallocTray_st * mallocTray_Args=malloc(sizeof(struct Args_MallocTray_st));
		mallocTray_Args->l_taskIdx=l_taskIdx;
		mallocTray_Args->trayIdx=trayIdx;
		mallocTray_Args->bufferSize=bufferSize;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_taskIdx, _OMPI_XclMallocTray, (void*)mallocTray_Args);
	}
	//return _OMPI_XclMallocTray(g_taskIdx, trayIdx, bufferSize, comm);
	return 0;
}

int OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm){
	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;

		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_FreeTray_st * freeTray_Args=malloc(sizeof(struct Args_FreeTray_st));
		freeTray_Args->l_taskIdx=l_taskIdx;
		freeTray_Args->trayIdx=trayIdx;

	}
	//return _OMPI_XclFreeTray(g_taskIdx, trayIdx, comm);
	return 0;
}


int OMPI_XclSend(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task,
		int g_dest_task, int tgID, MPI_Comm comm){

	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_src_task].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_src_task= g_taskList[g_src_task].l_taskIdx;
		//3.- Wrap the setProcedure_Args_st with the call parameters
		struct Args_Send_st * send_Args=malloc(sizeof(struct Args_Send_st));
		send_Args->trayIdx=trayIdx;
		send_Args->count=count;
		send_Args->MPIentityType=MPIentityType;
		send_Args->l_src_task=l_src_task;
		send_Args->g_dest_task=g_dest_task;
		send_Args->tgID=tgID;
		send_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_src_task, _OMPI_XclSend, (void*)send_Args);

	}

	return 0;
}

int OMPI_XclRecv(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task,
		int g_recv_task, int tgID,MPI_Comm comm) {
	//1.- Query if this process thread is involved in the operation
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_recv_task].r_rank) {
		//2.- Get the appropriate thread from the int thID_Of_(g_selTask)
		int l_recv_task= g_taskList[g_recv_task].l_taskIdx;
		//3.- Wrap the setProcedure_Args_st with the call parameters

		struct Args_Recv_st * recv_Args=malloc(sizeof(struct Args_Recv_st));
		recv_Args->trayIdx=trayIdx;
		recv_Args->count=count;
		recv_Args->MPIentityType=MPIentityType;
		recv_Args->g_src_task=g_src_task;
		recv_Args->l_recv_task=l_recv_task;
		recv_Args->tgID=tgID;
		recv_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_recv_task, _OMPI_XclRecv, (void*)recv_Args);

	}
	return 0;
}


/*int OMPI_XcltestF(int gsrctask, int srctrayIdx,int gdsttask, int dsttrayIdx, int count, int opTag, MPI_Comm comm){
	return 0;
}*/

int OMPI_XcltestF(int gsrctask, MPI_Comm comm, int srctrayIdx, int gdsttask, int dsttrayIdx , int traySize, int tgId){
	return 0;
}

int OMPI_XclSendRecv( MPI_Comm comm, int g_src_task, int src_trayIdx, int g_dst_task, int dst_trayIdx, int traySize, int tgID){

		static int firstIni=1;
	if(firstIni){
		sem_init(&FULL,0,0);
		pthread_mutex_init(&opTicket.mtx,NULL);
		pthread_cond_init(&opTicket.cond,NULL);
		list List=NULL;
		firstIni=0;
	}


	int myRank;
	MPI_Comm_rank(comm, &myRank);
	int l_src_task, l_dst_task;
	//int entityTypeSz;
	//MPI_Type_size(MPIentityType, &entityTypeSz);
	//int cpyBuffSz= count * entityTypeSz;
	int cpyBuffSz=traySize;
	int cpyMode;


	if(myRank == g_taskList[g_src_task].r_rank
			|| myRank == g_taskList[g_dst_task].r_rank){


		if(myRank == g_taskList[g_src_task].r_rank
				&& myRank == g_taskList[g_dst_task].r_rank){ //Same Node

			l_src_task = g_taskList[g_src_task].l_taskIdx;
			l_dst_task = g_taskList[g_dst_task].l_taskIdx;


			cpyMode=INTERDEVICE;

			if(l_taskList[l_src_task].device[0].deviceId==
					l_taskList[l_dst_task].device[0].deviceId){ //Same Device

				cpyMode=INTRADEVICE;

			}

		}
		else{
			cpyMode=INTERNODE;
		}


		//Execution

		switch(cpyMode){

		case INTRADEVICE:

			printf("IntraDevice, tag: %d\n",tgID);
			struct Args_matchedProducer_st * intracpyProducer_Args=malloc(sizeof(struct Args_matchedProducer_st));
			intracpyProducer_Args->tgID=tgID;
			intracpyProducer_Args->FULL=&FULL;
			intracpyProducer_Args->l_taskIdx=l_src_task;
			intracpyProducer_Args->dataSize=cpyBuffSz;
			intracpyProducer_Args->trayId=src_trayIdx;
			intracpyProducer_Args->ticket=&opTicket;

			//4.- Delegate the call to the thread.
			addSubRoutine(l_src_task, _intraDevCpyProducer, (void*)intracpyProducer_Args);

			struct Args_IntraMatchedConsumer_st * intraCpyConsumer_Args=malloc(sizeof(struct Args_IntraMatchedConsumer_st));
			intraCpyConsumer_Args->tgID=tgID;
			intraCpyConsumer_Args->FULL=&FULL;
			intraCpyConsumer_Args->l_src_taskIdx=l_src_task;
			intraCpyConsumer_Args->l_dst_taskIdx=l_dst_task;
			intraCpyConsumer_Args->dataSize=cpyBuffSz;
			intraCpyConsumer_Args->src_trayId=src_trayIdx;
			intraCpyConsumer_Args->dst_trayId=dst_trayIdx;
			intraCpyConsumer_Args->ticket=&opTicket;

			//4.- Delegate the call to the thread.
			addSubRoutine(l_dst_task, _intraDevCpyConsumer, (void*)intraCpyConsumer_Args);


			break;



		case INTERDEVICE:

			printf("InterDevice, tag: %d\n",tgID);
			struct Args_matchedProducer_st * cpyProducer_Args=malloc(sizeof(struct Args_matchedProducer_st));
			cpyProducer_Args->tgID=tgID;
			cpyProducer_Args->FULL=&FULL;
			cpyProducer_Args->l_taskIdx=l_src_task;
			cpyProducer_Args->dataSize=cpyBuffSz;
			cpyProducer_Args->trayId=src_trayIdx;
			cpyProducer_Args->ticket=&opTicket;

			//4.- Delegate the call to the thread.
			addSubRoutine(l_src_task, _interDevCpyProducer, (void*)cpyProducer_Args);

			struct Args_matchedConsumer_st * cpyConsumer_Args=malloc(sizeof(struct Args_matchedConsumer_st));
			cpyConsumer_Args->tgID=tgID;
			cpyConsumer_Args->FULL=&FULL;
			cpyConsumer_Args->l_taskIdx=l_dst_task;
			cpyConsumer_Args->dataSize=cpyBuffSz;
			cpyConsumer_Args->trayId=dst_trayIdx;
			cpyConsumer_Args->ticket=&opTicket;

			//4.- Delegate the call to the thread.
			addSubRoutine(l_dst_task, _interDevCpyConsumer, (void*)cpyConsumer_Args);


			break;

		case INTERNODE:
		/*	if(myRank == g_taskList[g_src_task].r_rank){ //Sender
				l_src_task = g_taskList[g_src_task].l_taskIdx;

						//3.- Wrap the setProcedure_Args_st with the call parameters (wrap usually done in the c_Interface.c )
						struct Args_Send_st * send_Args=malloc(sizeof(struct Args_Send_st));
						send_Args->trayIdx=src_trayIdx;
						send_Args->count=count;
						send_Args->MPIentityType=MPIentityType;
						send_Args->l_src_task=l_src_task;
						send_Args->g_dest_task=g_dst_task;
						send_Args->tgID=tgID;
						send_Args->comm=comm;

						//4.- Delegate the call to the thread.
						addSubRoutine(l_src_task, _OMPI_XclSend, (void*)send_Args);
			}else{ //Receiver

				l_dst_task = g_taskList[g_dst_task].l_taskIdx;
						struct Args_Recv_st * recv_Args=malloc(sizeof(struct Args_Recv_st));
						recv_Args->trayIdx=dst_trayIdx;
						recv_Args->count=count;
						recv_Args->MPIentityType=MPIentityType;
						recv_Args->g_src_task=g_src_task;
						recv_Args->l_recv_task=l_dst_task;
						recv_Args->tgID=tgID;
						recv_Args->comm=comm;

						//4.- Delegate the call to the thread.
						addSubRoutine(l_dst_task, _OMPI_XclRecv, (void*)recv_Args);
			}*/

			break;

		default:
				printf("No transference mode.\n");
				exit(-1);
			break;
		}
	}// If no rank participation just return
	return 0;
}




int OMPI_XclGather(int trayIdx, int count, MPI_Datatype MPIentityType,void **hostbuffer,
		const char* datafileName , MPI_Comm comm){
	return _OMPI_XclGather(trayIdx, count, MPIentityType,hostbuffer,datafileName ,comm);
}
int OMPI_XclScatter(const char* datafileName, int* count, MPI_Datatype MPIentityType, void* hostbuffer, int trayIdx, MPI_Comm comm){
	return _OMPI_XclScatter(datafileName, count, MPIentityType, hostbuffer, trayIdx, comm);
}

int NON_DELEGATED OMPI_XclWaitFor(int numTasks, int* taskIds, MPI_Comm comm){
	return _OMPI_XclWaitFor(numTasks, taskIds, comm);
}

int OMPI_XclWaitAllTasks(MPI_Comm comm){
	return _OMPI_XclWaitAllTasks(comm);
}


