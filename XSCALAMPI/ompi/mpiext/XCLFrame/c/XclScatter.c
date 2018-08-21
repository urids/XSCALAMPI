

#include "XclScatter.h"


//TODO: Scatter must be done with load balancing!!!
int OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype){

	MPI_Type_create_struct(blockcount, blocklen, displacements, basictypes, newDatatype);
	MPI_Type_commit(newDatatype);

	entityData.blockCount=blockcount;
	entityData.blocklen=blocklen;
	entityData.displacements=displacements;
	entityData.basictypes=basictypes;
	entityData.newDatatype=newDatatype;

	return MPI_SUCCESS;
}


int OMPI_XclScatter(MPI_Comm communicator, const char* fileName, MPI_Datatype MPIentityType, void **entitiesbuffer,int size, int* ePerRank){

	int i,j;
	int commsize,myRank;
	MPI_Status status;
	MPI_Comm_rank(communicator,&myRank);
	MPI_Comm_size(communicator, &commsize);

    MPI_File dataFile;
    MPI_Offset filesize;
    MPI_File_open(communicator, fileName, MPI_MODE_RDONLY, MPI_INFO_NULL, &dataFile);
    MPI_File_get_size(dataFile, &filesize);

    MPI_Type_size(MPIentityType, &MPIentityTypeSize);
    int numEntities=(int)filesize/MPIentityTypeSize;
    entitiesPerRank=numEntities/commsize;


    if(!entitiesbuffer){
    	*ePerRank=entitiesPerRank;
    	return 0;
    }
    else{
    	MPI_File_set_view(dataFile, myRank*entitiesPerRank*MPIentityTypeSize,
    	    					MPI_FLOAT, MPIentityType, "native", MPI_INFO_NULL);//TODO: check mpi etype.
    	//TODO: be aware of stack overflow.
    	MPI_File_read(dataFile, *entitiesbuffer, entitiesPerRank, MPIentityType, &status);

    	int entitiesBufferSize=entitiesPerRank*MPIentityTypeSize;

    	void * meminitHandle=NULL;
    	void (*initEntitiesBuffer)(int,int,int);
    	void (*writeEntitiesBuffer)(int,int,int,void**);
    	char *error;

    	meminitHandle=dlopen("libdvMgmt.so",RTLD_NOW);

    	if(!meminitHandle){
    		perror("library not found or could not be opened __base__");
    		exit(1);
    	}

    	initEntitiesBuffer = dlsym(meminitHandle, "initEntitiesBuffer");
    	writeEntitiesBuffer=dlsym(meminitHandle,"writeEntitiesBuffer");
    	if ((error = dlerror()) != NULL ) {
    		fputs(error, stderr);
    		exit(1);
    	}

    	(*initEntitiesBuffer)(numTasks, entitiesBufferSize,MPIentityTypeSize);
    	(*writeEntitiesBuffer)(numTasks,entitiesBufferSize,MPIentityTypeSize,entitiesbuffer);

    	MPI_File_close(&dataFile);
    return 0;
    }

}


int OMPI_XclGather(MPI_Comm comm, int entitiesBufferSize,void **entitiesbuffer){

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
}

