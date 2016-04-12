
#include "singleSidedComms.h"
///TODO: should I enable pass a rank where we want to read such data?
///TODO: should I enable pass a rank where host buffer lives?
int OMPI_XclReadTaskBuffer(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		void * memReadHandle = NULL;
		int (*readBuffer)(int taskIdx, int trayIdx, int bufferSize,
				void * hostBuffer);
		char *error;
		memReadHandle = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

		if (!memReadHandle) {
			perror("library not found or could not be opened AT: OMPI_XclSend");
			exit(1);
		}

		readBuffer = dlsym(memReadHandle, "readBuffer");
		if ((error = dlerror()) != NULL) {
			printf("err:");
			fputs(error, stderr);
			exit(1);
		}

		//convert the global task Id into a local to recover the data.
		//we already have verified that this data lives in this rank with the first IF upstream=)!!
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*readBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	}

	return 0;
}

//TODO: should I pass a rank to enable say on which task lives host buffer?
//TODO: REVIEW should I enable pass a rank where host buffer lives? =|
//TODO: SEVERE "Intra device copy do not automates buffer allocation =("

int OMPI_XclWriteTaskBuffer(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		void * memWrtHandle = NULL; //function pointer
		int (*initNewBuffer)(int l_taskIdx, int trayIdx, int bufferSize);
		int (*writeBuffer)(int l_taskIdx, int trayIdx, int bufferSize,
				void * hostBuffer);

		char *error;
		memWrtHandle = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

		if (!memWrtHandle) {
			perror("library not found or could not be opened AT: OMPI_XclWriteTaskBuffer");
			exit(1);
		}

		initNewBuffer = dlsym(memWrtHandle, "initNewBuffer");
		writeBuffer = dlsym(memWrtHandle, "writeBuffer");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}

		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
		(*writeBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	}
	return 0;
}

int OMPI_XclMallocTaskBuffer(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
		void * memHandle = NULL; //function pointer
		int (*initNewBuffer)(int l_taskIdx, int trayIdx, int bufferSize);

		char *error;
		memHandle = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

		if (!memHandle) {
			perror("library not found or could not be opened AT: OMPI_XclMallocTaskBuffer");
			exit(1);
		}

		initNewBuffer = dlsym(memHandle, "initNewBuffer");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
	}
	return 0;
}



int OMPI_XclFreeTaskBuffer(int g_taskIdx, int trayIdx, MPI_Comm comm) {
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {

		void * libHandle = NULL;
		void (*XclFreeTaskBuffer)(int, int);
		char *error;

		libHandle = dlopen("libmultiDeviceMgmt.so", RTLD_LAZY);

		if (!libHandle) {
			printf("library not found or could not be opened %d, %d", __FILE__,__LINE__);
			exit(1);
		}

		XclFreeTaskBuffer = dlsym(libHandle, "XclFreeTaskBuffer");
		if ((error = dlerror()) != NULL) {
			fputs(error, stderr);
			exit(1);
		}
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*XclFreeTaskBuffer)(l_taskIdx, trayIdx);

	}
	return 0;
}


int OMPI_XclSend(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task, int g_dest_task, int TAG, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_src_task].r_rank) {
		void* tmpBuffData;
		MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO:  this might conflict with scatter measure.
		int tmpBuffSz = count * MPIentityTypeSize;
		tmpBuffData = (void*) malloc(tmpBuffSz);

		void * memSendHandle = NULL;
		int (*readBuffer)(int taskId, int bufferSize, int memIdx,
				void * entitiesbuffer);
		char *error;
		memSendHandle = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

		if (!memSendHandle) {
			perror("library not found or could not be opened AT: OMPI_XclSend");
			exit(1);
		}

		readBuffer = dlsym(memSendHandle, "readBuffer");
		if ((error = dlerror()) != NULL) {
			printf("err:");
			fputs(error, stderr);
			exit(1);
		}

		//GET the data from the device.
		int l_src_task = g_taskList[g_src_task].l_taskIdx;
		(*readBuffer)(l_src_task, trayIdx, tmpBuffSz, tmpBuffData);
		//Send to the appropriate rank.
		int dest_rank = g_taskList[g_dest_task].r_rank;
		int ack;
		MPI_Request request;
		MPI_Status status;
		MPI_Isend(tmpBuffData, count, MPIentityType, dest_rank, TAG, comm,&request);
		 //printf("hello there user, I've just started this send\n\
		and I'm having a good time relaxing.\n");
		//MPI_Recv(&ack,1,MPI_INT,dest_rank,88,comm,MPI_STATUS_IGNORE);
		//MPI_Wait(&request,&status);

	}
	//TODO:free tmpBuffData.
	return 0;

}


int OMPI_XclRecv(int trayIdx, int count, MPI_Datatype MPIentityType, int g_src_task, int g_recv_task, int TAG,MPI_Comm comm) {
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_recv_task].r_rank) {
		void* tmpBuffData;
		MPI_Type_size(MPIentityType, &MPIentityTypeSize);//TODO: this might conflict with scatter measure.
		int tmpBuffSz = count * MPIentityTypeSize; //buffer size in bytes.
		tmpBuffData = (void*) malloc(tmpBuffSz);

		//recv from the appropriate rank.
		int src_rank = g_taskList[g_src_task].r_rank;
		int ack=1;
		MPI_Request request;
		MPI_Status status;
		MPI_Irecv(tmpBuffData, count, MPIentityType, src_rank, TAG, comm,&request);
		//MPI_Recv(tmpBuffData, count, MPIentityType, src_rank, TAG, comm,MPI_STATUS_IGNORE);
		//printf("hello there user, I've just started this receive\n\
		on, and I'm having a good time relaxing.\n");
		//MPI_Send(&ack,1,MPI_INT,0,88,comm);

		MPI_Wait(&request,&status);

		void * memRecvHandle = NULL; //function pointer
		int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
		int (*writeBuffer)(int taskId, int trayIdx, int bufferSize,
				void * hostBuffer);

		char *error;
		memRecvHandle = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

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

		int l_recv_task = g_taskList[g_recv_task].l_taskIdx;
		(*initNewBuffer)(l_recv_task, trayIdx, tmpBuffSz);
		(*writeBuffer)(l_recv_task, trayIdx, tmpBuffSz, tmpBuffData);
	}
	return 0;
}

int OMPI_XclSendRecv(int src_task, int src_trayIdx,
		             int dest_task, int dest_trayIdx,
		             int count, MPI_Datatype MPIentityType,
		             MPI_Comm comm) {
	int TAG = 0;
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

		//insert here sendRecvProcedure =).

		if (myRank == g_taskList[src_task].r_rank
						&& myRank == g_taskList[dest_task].r_rank  //if ranks match
						&& l_taskList[l_src_task].device[0].deviceId
								== l_taskList[l_dst_task].device[0].deviceId) { //if device match
						//then call intra-device copy.
					void * libHandler = NULL; //function pointer
					int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
					int (*intracpyBuffer)(int src_taskId, int srcTrayIdx,
							int dst_taskId, int dstTrayIdx, int bufferSize);

					char *error;
					libHandler = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

					if (!libHandler) {
						perror(
								"library multiDeviceMgmt not found or could not be opened AT: OMPI_XclSendRecv");
						exit(1);
					}

					initNewBuffer = dlsym(libHandler, "initNewBuffer");
					intracpyBuffer = dlsym(libHandler, "intracpyBuffer");
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

					OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,
							TAG, comm);
					OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,
							dest_task, TAG, comm);
				}

		//END sendRecvProcedure =).

		if (myRank == g_taskList[dest_task].r_rank) {
			MPI_Request profRequest;
			char* SndRcvProfMsg=malloc(1*sizeof(MPI_CHAR));
			SndRcvProfMsg[0]='a';
			int tag=19;
			MPI_Isend(SndRcvProfMsg, 1, MPI_CHAR, g_taskList[src_task].r_rank, tag, comm,
					&profRequest);
			//printf("I rank: %d have send to %d\n",g_taskList[dest_task].r_rank,g_taskList[src_task].r_rank);
		}
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

	} else {

		if (myRank == g_taskList[src_task].r_rank
				&& myRank == g_taskList[dest_task].r_rank  //if ranks match
				&& l_taskList[l_src_task].device[0].deviceId
						== l_taskList[l_dst_task].device[0].deviceId) { //if device match
				//then call intra-device copy.
			void * libHandler = NULL; //function pointer
			int (*initNewBuffer)(int taskIdx, int trayIdx, int bufferSize);
			int (*intracpyBuffer)(int src_taskId, int srcTrayIdx,
					int dst_taskId, int dstTrayIdx, int bufferSize);

			char *error;
			libHandler = dlopen("libmultiDeviceMgmt.so", RTLD_NOW);

			if (!libHandler) {
				perror(
						"library multiDeviceMgmt not found or could not be opened AT: OMPI_XclSendRecv");
				exit(1);
			}

			initNewBuffer = dlsym(libHandler, "initNewBuffer");
			intracpyBuffer = dlsym(libHandler, "intracpyBuffer");
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

			OMPI_XclSend(src_trayIdx, count, MPIentityType, src_task, dest_task,
					TAG, comm);
			OMPI_XclRecv(dest_trayIdx, count, MPIentityType, src_task,
					dest_task, TAG, comm);
		}
	}//end else ->not profiling
	return 0;

}
