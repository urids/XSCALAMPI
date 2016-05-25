#include "dataManager.h"

///TODO: should I enable pass a rank where we want to read such data?
///TODO: should I enable pass a rank where host buffer lives?
//TODO: I think no. but I should take care of perform a transparent SendRecv
		//to complete the operation where it was requested =) !!

int _OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
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
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*readBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	}

	return 0;
}

//TODO: should I pass a rank to enable say on which task lives host buffer?
//TODO: REVIEW should I enable pass a rank where host buffer lives? =|
//TODO: SEVERE "Intra device copy does not automates buffer allocation =("

int _OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
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

		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
		(*writeBuffer)(l_taskIdx, trayIdx, bufferSize, hostBuffer);
	}
	return 0;
}

int _OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {
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
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*initNewBuffer)(l_taskIdx, trayIdx, bufferSize);
	}
	return 0;
}


int _OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm) {
	int myRank;
	MPI_Comm_rank(comm, &myRank);
	if (myRank == g_taskList[g_taskIdx].r_rank) {

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
		int l_taskIdx= g_taskList[g_taskIdx].l_taskIdx;
		(*XclFreeTaskBuffer)(l_taskIdx, trayIdx);

	}
	return 0;
}
