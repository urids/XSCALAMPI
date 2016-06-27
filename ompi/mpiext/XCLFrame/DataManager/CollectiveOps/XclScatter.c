

#include "collectiveOps.h"

//TODO: Scatter must be done with load balancing!!!
//TODO: Scatter not need to be the first data distribution operation.
int ePerRank;




int ALL_TASKS_writeBuffer(int numTasks, int trayIdx, void * hostBuffer ) {
	int i,j; //index variables
	int status=0;

	void* tmpBuffer=hostBuffer;
	for (i = 0; i < numTasks; i++) {
		cl_event memProfileEvent;
		cl_ulong time_start, time_end;
		double total_time;

		int myRack=l_taskList[i].Rack;
				status = clEnqueueWriteBuffer(l_taskList[i].device[0].queue,
								l_taskList[i].device[0].memHandler[myRack][trayIdx], CL_TRUE,0,
								tskbufferSize[i], //extern DbuffersSize is declared in bufferFunctions.
								tmpBuffer,
								0, NULL, &memProfileEvent);
				tmpBuffer+=tskbufferSize[0]; //each task writing must do an offset.
			chkerr(status, "Writing mem Buffers", __FILE__, __LINE__);
			debug_print("copied: %zd \n" ,l_taskList[i].trayInfo->size);



#if PROFILE
		clWaitForEvents(1, &memProfileEvent);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_START,
				sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(memProfileEvent, CL_PROFILING_COMMAND_END,
				sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		profile_print("SCATTER buffer writing in task: %d  took -->  %0.3f ms\n", i,
				(total_time / 1000000.0));
#endif //if PROFILE


	}

	return status;
}
//count is of type pointer because in this same address we could return the number of entities per Rank =)!!.
int XclScatter(const char* datafileName, int* count, MPI_Datatype MPIentityType, void* hostbuffer, int trayIdx, MPI_Comm comm){
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

