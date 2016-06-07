#include "dataManager.h"


//int _OMPI_XclSend(int trayIdx, int count, MPI_Datatype MPIentityType, int l_src_task, int g_dest_task, int TAG, MPI_Comm comm){
int _OMPI_XclSend(void* Args){
	//1.- unwrap Args.

	struct Args_Send_st * send_Args=(struct Args_Send_st*) Args;
	int trayIdx 	 =send_Args->trayIdx;
	int count		 =send_Args->count;
	MPI_Datatype MPIentityType=send_Args->MPIentityType;
	int l_src_task   =send_Args->l_src_task;
	int g_dest_task  =send_Args->g_dest_task;
	int TAG			 =send_Args->TAG;
	MPI_Comm comm	 =send_Args->comm;


	//2.-The task thread makes the calls to the components

	void* tmpBuffData;
	MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO:  this might conflict with scatter measure.
	int tmpBuffSz = count * MPIentityTypeSize;
	tmpBuffData = (void*) malloc(tmpBuffSz);

	void * memSendHandle = NULL;
	int (*readBuffer)(int taskId, int bufferSize, int memIdx,
			void * entitiesbuffer);
	char *error;
	memSendHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!memSendHandle) {
		perror("library not found or could not be opened AT: OMPI_XclSend");
		exit(1);
	}

	readBuffer = dlsym(memSendHandle, "readBuffer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	//GET the data from the device.
	pthread_mutex_lock(&deviceQueueMutex);
	(*readBuffer)(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);
	pthread_mutex_unlock(&deviceQueueMutex);
	//Send to the appropriate rank.
	int dest_rank = g_taskList[g_dest_task].r_rank;
	int ack;
	MPI_Request request;
	MPI_Status status;
	int flag=0;
	MPI_Issend(tmpBuffData, count, MPIentityType, dest_rank, TAG, comm, &request);
	while (!flag)
	{
		MPI_Test(&request, &flag, &status);
	}

	//MPI_Send(tmpBuffData, count, MPIentityType, dest_rank, TAG, comm);

	//printf("hello there user, I've just started this send\n\
	and I'm having a good time relaxing.\n");
	//MPI_Recv(&ack,1,MPI_INT,dest_rank,88,comm,MPI_STATUS_IGNORE);
	//MPI_Wait(&request,&status);


	//TODO:free tmpBuffData after ensure send is complete (MPI_test or Wait).
	return 0;

}


//int _OMPI_XclRecv(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task, int l_recv_task, int TAG,MPI_Comm comm){
int _OMPI_XclRecv(void* Args){
	//1.- unwrap the Args.
	struct Args_Recv_st * recv_Args=(struct Args_Recv_st*)Args;

	int trayIdx		 =recv_Args->trayIdx;
	int count		 =recv_Args->count;
	MPI_Datatype MPIentityType = recv_Args->MPIentityType;
	int g_src_task   =recv_Args->g_src_task;
	int l_recv_task  =recv_Args->l_recv_task;
	int TAG			 =recv_Args->TAG;
	MPI_Comm comm	 =recv_Args->comm;

	//2.-The task thread makes the calls to the components
	void* tmpBuffData;
	MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO: this might conflict with scatter measure.
	int tmpBuffSz = count * MPIentityTypeSize; //buffer size in bytes.
	tmpBuffData = (void*) malloc(tmpBuffSz);

	//recv from the appropriate rank.
	int src_rank = g_taskList[g_src_task].r_rank;
	int ack=1;
	MPI_Request request;
	MPI_Status status;
	int flag=0;
	MPI_Irecv(tmpBuffData, count, MPIentityType, src_rank, TAG, comm,&request);
	MPI_Test(&request, &flag, &status);
	while (!flag)
	{
		MPI_Test(&request, &flag, &status);
	}

	//MPI_Recv(tmpBuffData, count, MPIentityType, src_rank, TAG, comm,NULL);

	//MPI_Recv(tmpBuffData, count, MPIentityType, src_rank, TAG, comm,MPI_STATUS_IGNORE);
	//printf("hello there user, I've just started this receive\n\
	on, and I'm having a good time relaxing.\n");
	//MPI_Send(&ack,1,MPI_INT,0,88,comm);

	//--MPI_Wait(&request,&status);

	void * memRecvHandle = NULL; //function pointer
	int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
	int (*writeBuffer)(int taskId, int trayIdx, int bufferSize,
			void * hostBuffer);

	char *error;
	memRecvHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!memRecvHandle) {
		perror("library not found or could not be opened AT: OMPI_XclRecv");
		exit(1);
	}

	initNewBuffer = dlsym(memRecvHandle, "initNewBuffer");
	writeBuffer = dlsym(memRecvHandle, "writeBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}


	pthread_mutex_lock(&deviceQueueMutex);
	(*initNewBuffer)(l_recv_task, trayIdx, tmpBuffSz);
	(*writeBuffer)(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);
	pthread_mutex_unlock(&deviceQueueMutex);

	return 0;
}



int _OMPI_XclSendRecv(int src_task, int src_trayIdx,
		int dest_task, int dest_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int TAG, MPI_Comm comm) {

	//int TAG = 0;
	int myRank;
	MPI_Comm_rank(comm, &myRank);


	/*
   // INTRA DEVICE COPY
  if (myRank == g_taskList[src_task].r_rank && myRank == g_taskList[dest_task].r_rank  //if ranks match
			&&
			l_taskList[l_src_task].device[0].deviceId== l_taskList[l_dst_task].device[0].deviceId) { //if devices match
		//then call intra-device copy.
		void * libHandler = NULL; //library pointer
		void * libCopyHandler = NULL; //library pointer
		int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
		int (*intracpyBuffer)(int src_taskId, int srcTrayIdx,
				int dst_taskId, int dstTrayIdx, int bufferSize);

		char *error;
		libHandler = dlopen("libbufferMgmt.so", RTLD_NOW);
		if (!libHandler) {
			perror(
					"library libdataManager not found or could not be opened AT: OMPI_XclSendRecv");
			exit(1);
		}

		initNewBuffer = dlsym(libHandler, "initNewBuffer");

		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		libCopyHandler = dlopen("libdataCopy.so", RTLD_NOW);
		if (!libCopyHandler) {
			perror(
					"library libdataCopy.so not found or could not be opened AT: OMPI_XclSendRecv");
			exit(1);
		}

		intracpyBuffer = dlsym(libCopyHandler, "intracpyBuffer");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		int entityTypeSz; //TODO: can I use MPIentityTypeSize.
		MPI_Type_size(MPIentityType, &entityTypeSz);
		int cpyBuffSz = count * entityTypeSz;
			//WARNING: Possible Race Condition against the task thread that owns each TRAY!!
		(*initNewBuffer)(l_dst_task, dest_trayIdx, cpyBuffSz);
		(*intracpyBuffer)(l_src_task, src_trayIdx, l_dst_task, dest_trayIdx,
				cpyBuffSz);

	} else { //perform the copy using MPI calls
	 */

	if (myRank == g_taskList[src_task].r_rank) {
		int l_src_task = g_taskList[src_task].l_taskIdx;

		//3.- Wrap the setProcedure_Args_st with the call parameters (wrap usually done in the c_Interface.c )
		struct Args_Send_st * send_Args=malloc(sizeof(struct Args_Send_st));
		send_Args->trayIdx=src_trayIdx;
		send_Args->count=count;
		send_Args->MPIentityType=MPIentityType;
		send_Args->l_src_task=l_src_task;
		send_Args->g_dest_task=dest_task;
		send_Args->TAG=TAG;
		send_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_src_task, _OMPI_XclSend, (void*)send_Args);
	}

	//_OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,TAG, comm);


	//3.- Wrap the setProcedure_Args_st with the call parameters

	if (myRank == g_taskList[dest_task].r_rank) {
		int l_dst_task = g_taskList[dest_task].l_taskIdx;
		struct Args_Recv_st * recv_Args=malloc(sizeof(struct Args_Recv_st));
		recv_Args->trayIdx=dest_trayIdx;
		recv_Args->count=count;
		recv_Args->MPIentityType=MPIentityType;
		recv_Args->g_src_task=src_task;
		recv_Args->l_recv_task=l_dst_task;
		recv_Args->TAG=TAG;
		recv_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_dst_task, _OMPI_XclRecv, (void*)recv_Args);

	}
	//_OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,	dest_task, TAG, comm);
	//} //END mpi involved data transfer calls


	//EXPERIMENTAL PART WITHOUT MPI
	/*--
	void* tmpBuffData;
	MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO:  this might conflict with scatter measure.
	int tmpBuffSz = count * MPIentityTypeSize;
	tmpBuffData = (void*) malloc(tmpBuffSz);

	void * buffHandle = NULL;
	int (*readBuffer)(int taskId, int bufferSize, int memIdx,
			void * entitiesbuffer);
	char *error;
	buffHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!buffHandle) {
		perror("library not found or could not be opened AT: OMPI_XclSend");
		exit(1);
	}

	readBuffer = dlsym(buffHandle, "readBuffer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	//GET the data from the device.
	pthread_mutex_lock(&deviceQueueMutex);
	(*readBuffer)(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);
	pthread_mutex_unlock(&deviceQueueMutex);

	//2.- Writing Part:

	int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
	int (*writeBuffer)(int taskId, int trayIdx, int bufferSize,
			void * hostBuffer);

	initNewBuffer = dlsym(buffHandle, "initNewBuffer");
	writeBuffer = dlsym(buffHandle, "writeBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}


	pthread_mutex_lock(&deviceQueueMutex);
	(*initNewBuffer)(l_recv_task, trayIdx, tmpBuffSz);
	(*writeBuffer)(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);
	pthread_mutex_unlock(&deviceQueueMutex);
	--*/


	return 0;
}


int _OMPI_P_XclSendRecv(int src_task, int src_trayIdx,
		int dest_task, int dest_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int TAG, MPI_Comm comm){

	//int TAG = 0;
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	int l_src_task = g_taskList[src_task].l_taskIdx;
	int l_dst_task = g_taskList[dest_task].l_taskIdx;

	char* profileFlag;
	int profilingEnabled = 0;
	profileFlag = getenv("XSCALA_PROFILING_APP");
	if (profileFlag != NULL) {
		profilingEnabled = 1;
	}


	if (profilingEnabled) {

		double tval_initSendRecvProfile, tval_finiSendRecvProfile, tval_totalSendRecvProfile;
		if(myRank == g_taskList[src_task].r_rank){
			tval_initSendRecvProfile= MPI_Wtime();
		}

		//init sendRecvProcedure =).

		if (myRank == g_taskList[src_task].r_rank
				&& myRank == g_taskList[dest_task].r_rank  //if ranks match
				&& l_taskList[l_src_task].device[0].deviceId
				== l_taskList[l_dst_task].device[0].deviceId) { //if device match
			//then call intra-device copy.
			void * libHandler = NULL; //function pointer
			void * libCopyHandler = NULL; //library pointer
			int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
			int (*intracpyBuffer)(int src_taskId, int srcTrayIdx,
					int dst_taskId, int dstTrayIdx, int bufferSize);

			char *error;
			libHandler = dlopen("libbufferMgmt.so", RTLD_NOW);

			if (!libHandler) {
				perror(
						"library multiDeviceMgmt not found or could not be opened AT: OMPI_XclSendRecv");
				exit(1);
			}

			initNewBuffer = dlsym(libHandler, "initNewBuffer");
			if ((error = dlerror()) != NULL) {
				fputs(error, stderr);
				exit(1);
			}

			libCopyHandler = dlopen("libdataCopy.so", RTLD_NOW);
			if (!libCopyHandler) {
				perror(
						"library libdataCopy.so not found or could not be opened AT: OMPI_XclSendRecv");
				exit(1);
			}

			intracpyBuffer = dlsym(libCopyHandler, "intracpyBuffer");
			if ((error = dlerror()) != NULL) {
				fputs(error, stderr);
				exit(1);
			}


			int entityTypeSz; //TODO: can I use MPIentityTypeSize.
			MPI_Type_size(MPIentityType, &entityTypeSz);
			int cpyBuffSz = count * entityTypeSz;

			(*initNewBuffer)(l_dst_task, dest_trayIdx, cpyBuffSz);
			(*intracpyBuffer)(l_src_task, src_trayIdx, l_dst_task, dest_trayIdx,
					cpyBuffSz);

		} else { //perform the copy using MPI

			//_OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,TAG, comm);
			//_OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,	dest_task, TAG, comm);
		} //END sendRecv Procedure Next is for profiling.


		//when the transfer is completed reply to the source with a simple flag "a"
		if (myRank == g_taskList[dest_task].r_rank) {
			MPI_Request profRequest;
			char* SndRcvProfMsg=malloc(1*sizeof(MPI_CHAR));
			SndRcvProfMsg[0]='a';
			int tag=19;
			MPI_Isend(SndRcvProfMsg, 1, MPI_CHAR, g_taskList[src_task].r_rank, tag, comm,
					&profRequest);
			//printf("I rank: %d have send to %d\n",g_taskList[dest_task].r_rank,g_taskList[src_task].r_rank);
		}
		//Sender blocks until receive the flag. Then reports the time that took.
		if (myRank == g_taskList[src_task].r_rank) {
			int tag = 19;
			MPI_Request profRecvrequest;
			MPI_Status status;
			void* SndRcvProfMsg=malloc(5*sizeof(MPI_CHAR));
			//MPI_Recv(SndRcvProfMsg,1,MPI_CHAR,g_taskList[dest_task].r_rank,tag,comm,&status);
			MPI_Irecv(SndRcvProfMsg, 1, MPI_CHAR, g_taskList[dest_task].r_rank, tag, comm,
					&profRecvrequest);
			MPI_Wait(&profRecvrequest, &status);
			tval_finiSendRecvProfile= MPI_Wtime();
			printf("SEND-RECV (%d-%d) took %f seconds\n",src_task,dest_task, tval_finiSendRecvProfile-tval_initSendRecvProfile);
		}

	}

	return 0;
}


///TODO: should I enable pass a rank where we want to read such data?
///TODO: should I enable pass a rank where host buffer lives?
//TODO: I think no. but I should take care of perform a transparent SendRecv
//to complete the operation where it was requested =) !!

//int _OMPI_XclReadTray(int l_taskIdx, int trayIdx, int bufferSize, void * hostBuffer){
int _OMPI_XclReadTray(void* Args){
	//1.- unwrap the Args.
	struct Args_ReadTray_st * readTray_Args=(struct Args_ReadTray_st*)Args;
	int l_taskIdx=readTray_Args->l_taskIdx;
	int trayIdx=readTray_Args->trayIdx;
	int bufferSize=readTray_Args->bufferSize;
	void* hostBuffer=readTray_Args->hostBuffer;

	//2.-The task thread makes the calls to the components
	void * memReadHandle = NULL;
	int (*readBuffer)(int taskIdx, int trayIdx, int bufferSize,void * hostBuffer);
	char *error;
	memReadHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!memReadHandle) {
		perror("library not found or could not be opened AT: _OMPI_XclReadTrays");
		exit(1);
	}

	readBuffer = dlsym(memReadHandle, "readBuffer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT Func: _OMPI_XclReadTray File %d",__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	//convert the global task Id into a local to recover the data.
	//we already have verified that this data lives in this rank with the first IF upstream=)!!
	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
	pthread_mutex_lock(&deviceQueueMutex);
	(*readBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	pthread_mutex_unlock(&deviceQueueMutex);

	return 0;
}

//TODO: should I pass a rank to enable say on which task lives host buffer?
//TODO: REVIEW should I enable pass a rank where host buffer lives? =|
//TODO: SEVERE "Intra device copy does not automates buffer allocation =("

//int _OMPI_XclWriteTray(int l_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
int _OMPI_XclWriteTray(void* Args){
	//1.- unwrap the Args.
	struct Args_WriteTray_st * writeTray_Args=(struct Args_WriteTray_st*)Args;
	int l_taskIdx=writeTray_Args->l_taskIdx;
	int trayIdx=writeTray_Args->trayIdx;
	int bufferSize=writeTray_Args->bufferSize;
	void* hostBuffer=writeTray_Args->hostBuffer;


	//2.-The task thread makes the calls to the components
	void * memWrtHandle = NULL; //function pointer
	int (*initNewBuffer)(int l_taskIdx, int trayIdx, int bufferSize);
	int (*writeBuffer)(int l_taskIdx, int trayIdx, int bufferSize,
			void * hostBuffer);

	char *error;
	memWrtHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!memWrtHandle) {
		perror("library not found or could not be opened AT: _OMPI_XclWriteTray");
		exit(1);
	}

	initNewBuffer = dlsym(memWrtHandle, "initNewBuffer");
	writeBuffer = dlsym(memWrtHandle, "writeBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}

	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
	pthread_mutex_lock(&deviceQueueMutex);
	(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
	(*writeBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	pthread_mutex_unlock(&deviceQueueMutex);

	return 0;
}

//int _OMPI_XclMallocTray(int l_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){
int _OMPI_XclMallocTray(void* Args){
	//1.- unwrap the Args.
	struct Args_MallocTray_st * mallocTray_Args=(struct Args_MallocTray_st*)Args;
	int l_taskIdx=mallocTray_Args->l_taskIdx;
	int trayIdx=mallocTray_Args->trayIdx;
	int bufferSize=mallocTray_Args->bufferSize;

	//2.-The task thread makes the calls to the components
	void * memHandle = NULL; //function pointer
	int (*initNewBuffer)(int l_taskIdx, int trayIdx, int bufferSize);

	char *error;
	memHandle = dlopen("libbufferMgmt.so", RTLD_NOW);

	if (!memHandle) {
		perror("library not found or could not be opened AT: _OMPI_XclMallocTray");
		exit(1);
	}

	initNewBuffer = dlsym(memHandle, "initNewBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	pthread_mutex_lock(&deviceQueueMutex);
	(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
	pthread_mutex_unlock(&deviceQueueMutex);
	return 0;
}


//int _OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm) {
int _OMPI_XclFreeTray(void* Args){
	//1.- unwrap the Args.
	struct Args_FreeTray_st * freeTray_Args=(struct Args_FreeTray_st*)Args;
	int l_taskIdx=freeTray_Args->l_taskIdx;
	int trayIdx=freeTray_Args->trayIdx;

	//2.-The task thread makes the calls to the components
	void * libHandle = NULL;
	void (*XclFreeTaskBuffer)(int, int);
	char *error;

	libHandle = dlopen("libbufferMgmt.so", RTLD_LAZY);

	if (!libHandle) {
		printf("library not found or could not be opened %d, %d", __FILE__,__LINE__);
		exit(1);
	}

	XclFreeTaskBuffer = dlsym(libHandle, "XclFreeTaskBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
	(*XclFreeTaskBuffer)(l_taskIdx, trayIdx);


	return 0;
}
