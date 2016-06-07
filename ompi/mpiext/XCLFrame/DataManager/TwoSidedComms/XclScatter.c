

#include "XclScatter.h"


//TODO: Scatter must be done with load balancing!!!
//TODO: Scatter not need to be the first data distribution operation.
int MPIentityTypeSize;
int ePerRank;

int _OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype){

	MPI_Type_create_struct(blockcount, blocklen, displacements, basictypes, newDatatype);
	MPI_Type_commit(newDatatype);

	entityData.blockCount=blockcount;
	entityData.blocklen=blocklen;
	entityData.displacements=displacements;
	entityData.basictypes=basictypes;
	entityData.newDatatype=newDatatype;

	return MPI_SUCCESS;
}


//int OMPI_XclScatter(MPI_Comm communicator, const char* fileName, MPI_Datatype MPIentityType, void **entitiesbuffer,int size, int* ePerRank, cl_mem * memObjects){

//count is of type pointer because in this same address we could return the number of entities per Rank =)!!.
int _OMPI_XclScatter(const char* datafileName, int* count, MPI_Datatype MPIentityType, void* hostbuffer, int trayIdx, MPI_Comm comm){
	int i,j;
	int commsize,myRank;
	MPI_Status status;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm, &commsize);

    MPI_File dataFile;
    MPI_Offset filesize;
    MPI_File_open(comm, datafileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &dataFile);
    MPI_File_get_size(dataFile, &filesize);

    MPI_Type_size(MPIentityType, &MPIentityTypeSize);
    int numEntities=(int)filesize/MPIentityTypeSize;
    ePerRank=(int)numEntities/commsize;

    if(!hostbuffer){
    	*count=ePerRank;
    	return 0;
    }
    else{// each process set a file view
    	MPI_File_set_view(dataFile, myRank*ePerRank*MPIentityTypeSize,
    	    					MPI_FLOAT, MPIentityType, "native", MPI_INFO_NULL);//TODO: check mpi etype is MPI_FLOAT right?.
    	//TODO: be aware of stack overflow hopefully count pointer has already solved this issue =).
    	MPI_File_read(dataFile, hostbuffer, ePerRank, MPIentityType, &status);
    	int HBufferSize=ePerRank*MPIentityTypeSize;

    	//read the libdvMgmt symbol
    	void * meminitHandle=NULL;
    	//void (*initEntitiesBuffer)(int,int,int, cl_mem*);
    	void (*initEntitiesBuffer)(int l_numTasks, int trayIdx, int HBufferSize, int MPIEntityTypeSz);
    	void (*writeEntitiesBuffer)(int ,int , void* bufferData);
    	char *error;

    	meminitHandle=dlopen("libdvMgmt.so",RTLD_NOW);

    	if(!meminitHandle){
    		perror("library not found or could not be opened __base__");
    		exit(1);
    	}

    	initEntitiesBuffer = dlsym(meminitHandle, "ALL_TASK_initBuffer");
    	writeEntitiesBuffer=dlsym(meminitHandle, "ALL_TASK_writeBuffer");
    	if ((error = dlerror()) != NULL ) {
    		fputs(error, stderr);
    		exit(1);
    	}

    	(*initEntitiesBuffer)(l_numTasks, trayIdx, HBufferSize, MPIentityTypeSize);
    	(*writeEntitiesBuffer)(l_numTasks, trayIdx , hostbuffer);

    	//MPI_File_close(&dataFile); //TODO: it is ready to be closed??
    return 0;

    }
}


//hostbuffer is a pointer to pointer to return a copy of the buffer (if user requests that.)!!
int _OMPI_XclGather(int trayIdx, int count, MPI_Datatype MPIentityType,void **hostbuffer,
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
/*int OMPI_XclGather(MPI_Comm comm, int entitiesBufferSize,void **entitiesbuffer){

	void * memgatherHandle=NULL;
	    	void (*readEntitiesBuffer)(int,int,int,void**);
	    	char *error;

	    	memgatherHandle=dlopen("libdvMgmt.so",RTLD_NOW);

	    	if(!memgatherHandle){
	    		perror("library not found or could not be opened __base__");
	    		exit(1);
	    	}

	    	readEntitiesBuffer=dlsym(memgatherHandle,"readEntitiesBuffer");
	    	if ((error = dlerror()) != NULL ) {
	    		fputs(error, stderr);
	    		exit(1);
	    	}

	    	(*readEntitiesBuffer)(numTasks,entitiesBufferSize,MPIentityTypeSize,entitiesbuffer);


	return 0;
}*/

