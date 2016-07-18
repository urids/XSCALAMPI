#include "dataManager.h"

pthread_mutex_t interNodeCommMtx=PTHREAD_MUTEX_INITIALIZER;

int _OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype){

	MPI_Type_create_struct(blockcount, blocklen, displacements, basictypes, newDatatype);
	MPI_Type_commit(newDatatype);

	entityData_t entityData;

	entityData.blockCount=blockcount;
	entityData.blocklen=blocklen;
	entityData.displacements=displacements;
	entityData.basictypes=basictypes;
	entityData.newDatatype=newDatatype;

	return MPI_SUCCESS;
}

//int _OMPI_XclSend(int trayIdx, int count, MPI_Datatype MPIentityType, int l_src_task, int g_dest_task, int tgID, MPI_Comm comm){
int _OMPI_XclSend(void* Args){
	//1.- unwrap Args.

	struct Args_Send_st * send_Args=(struct Args_Send_st*) Args;
	int trayIdx 	 =send_Args->trayIdx;
	int count		 =send_Args->count;
	MPI_Datatype MPIentityType=send_Args->MPIentityType;
	int l_src_task   =send_Args->l_src_task;
	int g_dest_task  =send_Args->g_dest_task;
	int tgID		 =send_Args->tgID;
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
	memSendHandle = dlopen("libdataCopy.so", RTLD_NOW);

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

	(*readBuffer)(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);

	//Send to the appropriate rank.


	int dest_rank = g_taskList[g_dest_task].r_rank;
	int ack;
	MPI_Request request;
	MPI_Status status;
//??pthread_mutex_lock(&interNodeCommMtx);
	int flag=0;
	MPI_Issend(tmpBuffData, count, MPIentityType, dest_rank, tgID, comm, &request);
	while (!flag)
	{
		MPI_Test(&request, &flag, &status);
	}
//??pthread_mutex_unlock(&interNodeCommMtx);

	//MPI_Send(tmpBuffData, count, MPIentityType, dest_rank, tgID, comm);
	//printf("hello there user, I've just started this send\n\
	and I'm having a good time relaxing.\n");
	//MPI_Recv(&ack,1,MPI_INT,dest_rank,88,comm,MPI_STATUS_IGNORE);
	//MPI_Wait(&request,&status);


	//TODO:free tmpBuffData after ensuring send is complete (MPI_test or Wait).





	return 0;

}


//int _OMPI_XclRecv(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task, int l_recv_task, int tgID,MPI_Comm comm){
int _OMPI_XclRecv(void* Args){
	//1.- unwrap the Args.
	struct Args_Recv_st * recv_Args=(struct Args_Recv_st*)Args;

	int trayIdx		 =recv_Args->trayIdx;
	int count		 =recv_Args->count;
	MPI_Datatype MPIentityType = recv_Args->MPIentityType;
	int g_src_task   =recv_Args->g_src_task;
	int l_recv_task  =recv_Args->l_recv_task;
	int tgID			 =recv_Args->tgID;
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
	MPI_Irecv(tmpBuffData, count, MPIentityType, src_rank, tgID, comm,&request);
	MPI_Test(&request, &flag, &status);
	while (!flag)
	{
		MPI_Test(&request, &flag, &status);
	}

	//MPI_Recv(tmpBuffData, count, MPIentityType, src_rank, tgID, comm,NULL);

	//MPI_Recv(tmpBuffData, count, MPIentityType, src_rank, tgID, comm,MPI_STATUS_IGNORE);
	//printf("hello there user, I've just started this receive\n\
	on, and I'm having a good time relaxing.\n");
	//MPI_Send(&ack,1,MPI_INT,0,88,comm);

	//MPI_Wait(&request,&status);

	void * bufferMgmtHandle = NULL; //function pointer
	void * dataCpyHandle = NULL; //function pointer
	int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
	int (*writeBuffer)(int taskId, int trayIdx, int bufferSize,
			void * hostBuffer);

	char *error;
	bufferMgmtHandle = dlopen("libbufferMgmt.so", RTLD_NOW);
	if (!bufferMgmtHandle) {
		perror("library bufferMgmt not found or could not be opened AT: OMPI_XclRecv");
		exit(1);
	}

	dataCpyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCpyHandle) {
		perror("library dataCopy not found or could not be opened AT: OMPI_XclRecv");
		exit(1);
	}

	initNewBuffer = dlsym(bufferMgmtHandle, "initNewBuffer");
	writeBuffer = dlsym(dataCpyHandle, "writeBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}

	(*initNewBuffer)(l_recv_task, trayIdx, tmpBuffSz);
	(*writeBuffer)(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);


	return 0;
}





int  _OMPI_XclSendRecv(int src_task, int src_trayIdx,
		int dest_task, int dest_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int tgID, MPI_Comm comm) {





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

	/*--	if (myRank == g_taskList[src_task].r_rank) {
		int l_src_task = g_taskList[src_task].l_taskIdx;

		//3.- Wrap the setProcedure_Args_st with the call parameters (wrap usually done in the c_Interface.c )
		struct Args_Send_st * send_Args=malloc(sizeof(struct Args_Send_st));
		send_Args->trayIdx=src_trayIdx;
		send_Args->count=count;
		send_Args->MPIentityType=MPIentityType;
		send_Args->l_src_task=l_src_task;
		send_Args->g_dest_task=dest_task;
		send_Args->tgID=tgID;
		send_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_src_task, _OMPI_XclSend, (void*)send_Args);
	}

	//_OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,tgID, comm);


	//3.- Wrap the setProcedure_Args_st with the call parameters

	if (myRank == g_taskList[dest_task].r_rank) {
		int l_dst_task = g_taskList[dest_task].l_taskIdx;
		struct Args_Recv_st * recv_Args=malloc(sizeof(struct Args_Recv_st));
		recv_Args->trayIdx=dest_trayIdx;
		recv_Args->count=count;
		recv_Args->MPIentityType=MPIentityType;
		recv_Args->g_src_task=src_task;
		recv_Args->l_recv_task=l_dst_task;
		recv_Args->tgID=tgID;
		recv_Args->comm=comm;

		//4.- Delegate the call to the thread.
		addSubRoutine(l_dst_task, _OMPI_XclRecv, (void*)recv_Args);

	}
	//_OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,	dest_task, tgID, comm);
	//} //END mpi involved data transfer calls
--*/



/**--	//READING PART.

	struct Args_waitSubroutine_st * waitSRCSubroutine_Args=malloc(sizeof(struct Args_waitSubroutine_st));
	waitSRCSubroutine_Args->semaphore=malloc(sizeof(sem_t));
	waitSRCSubroutine_Args->semaphore=&EMPTY;
	addSubRoutine(l_src_task, waitSubroutine, (void*)waitSRCSubroutine_Args);


	struct Args_ReadTray_st * readTray_Args=malloc(sizeof(struct Args_ReadTray_st));
	readTray_Args->l_taskIdx=l_src_task;
	readTray_Args->trayIdx=src_trayIdx;
	readTray_Args->bufferSize=tmpCpyBuffSz;
	readTray_Args->hostBuffer=tmpCpyBuffData;
	addSubRoutine(l_src_task, _OMPI_XclReadTray, (void*)readTray_Args);

	struct Args_signalSubroutine_st * signalSRCSubroutine_Args=malloc(sizeof(struct Args_signalSubroutine_st));
	signalSRCSubroutine_Args->semaphore=malloc(sizeof(sem_t));
	signalSRCSubroutine_Args->semaphore=&FULL;
	addSubRoutine(l_src_task, signalSubroutine, (void*)signalSRCSubroutine_Args);


	//WRITING PART.

	struct Args_waitSubroutine_st * waitDSTSubroutine_Args=malloc(sizeof(struct Args_waitSubroutine_st));
	waitDSTSubroutine_Args->semaphore=malloc(sizeof(sem_t));
	waitDSTSubroutine_Args->semaphore=&FULL;
	addSubRoutine(l_dst_task, waitSubroutine, (void*)waitDSTSubroutine_Args);

	struct Args_WriteTray_st * writeTray_Args=malloc(sizeof(struct Args_WriteTray_st));
	writeTray_Args->l_taskIdx=l_dst_task;
	writeTray_Args->trayIdx=dest_trayIdx;
	writeTray_Args->bufferSize=tmpCpyBuffSz;
	writeTray_Args->hostBuffer=tmpCpyBuffData;
	addSubRoutine(l_dst_task, _OMPI_XclWriteTray, (void*)writeTray_Args);

	struct Args_signalSubroutine_st * signalDSTSubroutine_Args=malloc(sizeof(struct Args_signalSubroutine_st));
	signalDSTSubroutine_Args->semaphore=malloc(sizeof(sem_t));
	signalDSTSubroutine_Args->semaphore=&EMPTY;
	addSubRoutine(l_dst_task, signalSubroutine, (void*)signalDSTSubroutine_Args);
	--**/

	return 0;
}

int _interNode(int src_task, int src_trayIdx,
		int dest_task, int dest_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int tgID, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);

	if(myRank == g_taskList[src_task].r_rank){ //I'm Src process

	}else{ //I'm dst process

	}
	return 0;
}

int _interDevCpyProducer(void* Args){

	void * dataCopyHandle = NULL;
	int (*interDevCpyProducer)(void* Args);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _interDevCpyProducer");
		exit(-1);
	}

	interDevCpyProducer = dlsym(dataCopyHandle, "interDevCpyProducer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	interDevCpyProducer(Args);

	return 0;

}

int _interDevCpyConsumer(void* Args){

	void * dataCopyHandle = NULL;
	int (*interDevCpyConsumer)(void* Args);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _intraDevCpyProducer");
		exit(-1);
	}

	interDevCpyConsumer = dlsym(dataCopyHandle, "interDevCpyConsumer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	interDevCpyConsumer(Args);

	return 0;

}


int _intraDevCpyProducer(void* Args){

	void * dataCopyHandle = NULL;
	int (*intraDevCpyProducer)(void* Args);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _intraDevCpyProducer");
		exit(-1);
	}

	intraDevCpyProducer = dlsym(dataCopyHandle, "intraDevCpyProducer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	intraDevCpyProducer(Args);

	return 0;

}

int _intraDevCpyConsumer(void* Args){

	void * dataCopyHandle = NULL;
	int (*intraDevCpyConsumer)(void* Args);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _interDevCpyProducer");
		exit(-1);
	}

	intraDevCpyConsumer = dlsym(dataCopyHandle, "intraDevCpyConsumer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	intraDevCpyConsumer(Args);

	return 0;

}

int _interNodeCpyProducer(void* Args){
	void * dataCopyHandle = NULL;
		int (*interNodeCpyProducer)(void* Args);
		char *error;

		dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
		if (!dataCopyHandle) {
			perror("library data  Copy not found or could not be opened AT: _interDevCpyProducer");
			exit(-1);
		}

		interNodeCpyProducer = dlsym(dataCopyHandle, "interNodeCpyProducer");
		if ((error = dlerror()) != NULL) {
			printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
			fputs(error, stderr);
			exit(1);
		}

		interNodeCpyProducer(Args);

		return 0;
}


int _interNodeCpyConsumer(void* Args){

	void * dataCopyHandle = NULL;
	int (*interNodeCpyConsumer)(void* Args);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _interDevCpyProducer");
		exit(-1);
	}

	interNodeCpyConsumer = dlsym(dataCopyHandle, "interNodeCpyConsumer");
	if ((error = dlerror()) != NULL) {
		printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
		fputs(error, stderr);
		exit(1);
	}

	interNodeCpyConsumer(Args);

	return 0;
}

/*
int interDevice(int src_task, int src_trayIdx,
		int dst_task, int dst_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int tgID){



		//READING PART
		struct Args_matchedProducer_st* matchedProducer_Args=malloc(sizeof(struct Args_matchedProducer_st));
		matchedProducer_Args->tgID=tgID;
		matchedProducer_Args->FULL=&FULL;
		matchedProducer_Args->l_taskIdx=l_src_task;
		matchedProducer_Args->dataSize=cpyBuffSz;
		matchedProducer_Args->trayId=src_trayIdx;
		matchedProducer_Args->ticket=&opTicket;

		addSubRoutine(l_src_task, _matchedProducer, (void*)matchedProducer_Args);

		//WRITING PART
		struct Args_matchedConsumer_st* matchedConsumer_Args=malloc(sizeof(struct Args_matchedConsumer_st));
			matchedConsumer_Args->tgID=tgID;
			matchedConsumer_Args->FULL=&FULL;
			matchedConsumer_Args->ticket=&opTicket;
			matchedConsumer_Args->l_taskIdx=l_dst_task;
			matchedConsumer_Args->dataSize=cpyBuffSz;
			matchedConsumer_Args->trayId=dst_trayIdx;

		addSubRoutine(l_dst_task, _matchedConsumer, (void*)matchedConsumer_Args);
	return 0;
}*/


int _OMPI_P_XclSendRecv(int src_task, int src_trayIdx,
		int dest_task, int dest_trayIdx,
		int count, MPI_Datatype MPIentityType,
		int tgID, MPI_Comm comm){

	//int tgID = 0;
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

			//_OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,tgID, comm);
			//_OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,	dest_task, tgID, comm);
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


	void * dataCopyHandle = NULL;
	int (*readBuffer)(int taskIdx, int trayIdx, int bufferSize,void * hostBuffer);
	char *error;

	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library data  Copy not found or could not be opened AT: _interDevCpyProducer");
		exit(-1);
	}
	readBuffer = dlsym(dataCopyHandle, "readBuffer");
		if ((error = dlerror()) != NULL) {
			printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
			fputs(error, stderr);
			exit(1);
	}

	//convert the global task Id into a local to recover the data.
	//we already have verified that this data lives in this rank with the first IF upstream=)!!
	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;


	(*readBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);


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


	void * dataCopyHandle = NULL;
	void * bufferMgmtHandle = NULL;
	int (*initNewBuffer)(int l_taskIdx, int trayIdx, int bufferSize);
	int (*writeBuffer)(int l_taskIdx, int trayIdx, int bufferSize,
			void * hostBuffer);

	char *error;

	bufferMgmtHandle = dlopen("libbufferMgmt.so", RTLD_NOW);
	if (!bufferMgmtHandle) {
		perror("library bufferMgmt not found or could not be opened AT: _OMPI_XclWriteTray");
		exit(1);
	}
	dataCopyHandle = dlopen("libdataCopy.so", RTLD_NOW);
	if (!dataCopyHandle) {
		perror("library dataCopy not found or could not be opened AT: _OMPI_XclWriteTray");
		exit(-1);
	}

	initNewBuffer = dlsym(bufferMgmtHandle, "initNewBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}

	writeBuffer = dlsym(dataCopyHandle, "writeBuffer");
		if ((error = dlerror()) != NULL) {
			printf("err: AT %d , %d ", __FUNCTION__ ,__FILE__);
			fputs(error, stderr);
			exit(1);
	}

	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;

	(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
	(*writeBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);


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

	(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);

	return 0;
}


int _OMPI_XclFreeTray(void* Args){
	//1.- unwrap the Args.
	struct Args_FreeTray_st * freeTray_Args=(struct Args_FreeTray_st*)Args;
	int l_taskIdx=freeTray_Args->l_taskIdx;
	int trayIdx=freeTray_Args->trayIdx;

	//2.-The task thread makes the calls to the components
	void * bufferMgmtHandle = NULL;
	void (*XclFreeTaskBuffer)(int, int);
	char *error;

	bufferMgmtHandle = dlopen("libbufferMgmt.so", RTLD_LAZY);

	if (!bufferMgmtHandle) {
		printf("library not found or could not be opened %d, %d", __FILE__,__LINE__);
		exit(1);
	}

	XclFreeTaskBuffer = dlsym(bufferMgmtHandle, "XclFreeTaskBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}
	//int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
	(*XclFreeTaskBuffer)(l_taskIdx, trayIdx);
	return 0;
}

int _OMPI_XclScatter(const char* datafileName, int* count, MPI_Datatype MPIentityType, int trayIdx, MPI_Comm comm){
	return 0;
}

int _OMPI_XclGather(int trayIdx, int count, MPI_Datatype MPIentityType,void **hostbuffer,
		const char* datafileName , MPI_Comm comm){
	return 0;
}




