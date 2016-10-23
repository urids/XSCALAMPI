
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
			pthread_join(thds[j], NULL); //?? WRONG!!! This way the thread is finished
		}
		//MPI_Barrier(comm);
		return 0;
}



int postBackCounter;
pthread_mutex_t  synchMutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   backCond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t   condN      = PTHREAD_COND_INITIALIZER;




void* synchSubroutine(void* args){
	synchArgs_t *largs=(synchArgs_t*)args;

	pthread_barrier_wait(&largs->localBarrier);
	//--when all threads pass the barrier
	//--on wakes up process thread
	pthread_mutex_lock(&synchMutex);//thdContention
	postBackCounter++;
	if(postBackCounter==largs->num_l_tasks){
		//pthread_mutex_lock(&condNMutex);
		pthread_cond_signal(&condN);
		//pthread_mutex_unlock(&condNMutex);
	}

	//each task thread waits the process thread finishes synchronization
	pthread_cond_wait(&backCond,&synchMutex);

	pthread_mutex_unlock(&synchMutex);
	return (void*)0;
}

void localSynch(int l_wTskSize, int* localIDs, MPI_Comm Comm){

	//INIT A NEW LOCAL SYNCH
	//0.- initialize a local the barrier and the postBack Counter
	postBackCounter=0;
	pthread_barrier_t mybarrier;
	pthread_barrier_init(&mybarrier,NULL,l_wTskSize);

	//1.- Ensure no task thread signals me prior to my wait
	pthread_mutex_lock(&synchMutex);

	//2.- delegate the synchSubRoutines
	synchArgs_t mySyncArgs={.localBarrier=mybarrier,.num_l_tasks=l_wTskSize};
	for(i=0;i<l_wTskSize;i++){
		addSubRoutine(localIDs[i],&synchSubroutine,(void*)&mySyncArgs);
	}
	//3.- WAIT
	pthread_cond_wait(&condN,&synchMutex);
	//printf("CALL");
	//4.- Synch (distributed) all processes TODO: create and synch a subgroup.
	MPI_Barrier(Comm);
	//5.-broadcast backCondition
	pthread_cond_broadcast(&backCond);
	//6.- unlock the synchMutex to let the task thread continue working.
	pthread_mutex_unlock(&synchMutex);

	pthread_barrier_destroy(&mybarrier);

}
