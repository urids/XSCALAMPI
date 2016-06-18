
#include "collectiveOps.h"

//hostbuffer is a pointer to pointer to return a copy of the buffer (if user requests that.)!!
int XclGather(int trayIdx, int count, MPI_Datatype MPIentityType,void **hostbuffer,
				   const char* datafileName , MPI_Comm comm){
	int i, j;
	int commsize, myRank;
	MPI_Status status;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm, &commsize);

	void *tmphostbuffer;
	tmphostbuffer=(void*)malloc(count * MPIentityTypeSize*l_numTasks);//space enough for all tasks in this process
	MPI_File dataFile;
	MPI_Offset filesize;

	//int HBufferSize = ePerRank * MPIentityTypeSize;

	//read the libdvMgmt symbol
	void * memRwHandle = NULL;
	void (*readEntitiesBuffer)(int, int, void*);
	char *error;

	memRwHandle = dlopen("libdvMgmt.so", RTLD_NOW);

	if (!memRwHandle) {
		perror("library not found or could not be opened __base__");
		exit(1);
	}

	readEntitiesBuffer = dlsym(memRwHandle, "ALL_TASKS_readBuffer");
	if ((error = dlerror()) != NULL) {
		fputs(error, stderr);
		exit(1);
	}

	(*readEntitiesBuffer)(l_numTasks, trayIdx, tmphostbuffer);

	MPI_File_open(comm, datafileName, MPI_MODE_RDWR | MPI_MODE_CREATE,
			MPI_INFO_NULL, &dataFile);
	MPI_File_get_size(dataFile, &filesize);
	//MPI_Type_size(MPIentityType, &MPIentityTypeSize);
	//int numEntities=(int)filesize/MPIentityTypeSize;
	//ePerRank=(int)numEntities/commsize;

	// each process set a file view TODO: Again??
	MPI_File_set_view(dataFile, myRank * ePerRank * MPIentityTypeSize,
			MPI_FLOAT, MPIentityType, "native", MPI_INFO_NULL );//TODO: check mpi etype is MPI_FLOAT right?.

	//TODO: be aware of stack overflow hopefully count pointer has already solved this issue =).
	MPI_File_write(dataFile, tmphostbuffer, ePerRank, MPIentityType, &status);

	if(hostbuffer!=NULL){
		*hostbuffer=tmphostbuffer;
	}else
	{
		free(tmphostbuffer);
	}

	MPI_File_close(&dataFile);
	return 0;

}

int ALL_TASKS_readBuffer(int numTasks, int trayIdx ,void * hostBuffer) {
	int i,j;
	int status=0;


	void* tmpBuffer = hostBuffer;
	for (i = 0; i < numTasks; i++) {
		int myRack=l_taskList[i].Rack;
		debug_print("read Request: %d \n" ,tskbufferSize[i]);
		debug_print("tmpBuffer %d \n" ,tmpBuffer);
		status = clEnqueueReadBuffer(l_taskList[i].device[0].queue,
				l_taskList[i].device[0].memHandler[myRack][trayIdx], CL_TRUE, 0, tskbufferSize[i],
				tmpBuffer, 0, NULL, NULL);
		tmpBuffer+=tskbufferSize[0];
		chkerr(status, "Reading mem Buffers", __FILE__, __LINE__);
	}

	return status;
}
