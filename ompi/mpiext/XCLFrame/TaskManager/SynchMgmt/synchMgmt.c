
#include "synchMgmt.h"

int XclWaitAllTasks(pthread_t* thds, int numTasks,MPI_Comm comm){
	int i;
	for(i=0;i<numTasks;i++){
				pthread_join(thds[i], NULL);
	}
	/*for(i=0;i<numTasks;i++){
		pthread_detach(thds[i]);
	}*/
	MPI_Barrier(comm);
	return 0;
}


int XclWaitFor(pthread_t* thds ,int l_numTasks, int* l_taskIds, MPI_Comm comm){
	int i,j;
	debug_print("waiting for %d \n", *l_taskIds);
		for(i=0;i<l_numTasks;i++){
			j=l_taskIds[i];
			pthread_join(thds[j], NULL);
		}
		//MPI_Barrier(comm);
		return 0;
}
