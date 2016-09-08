

#include "poolManager.h"

int readersCount=0;
pthread_mutex_t readTry=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readersContention=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resourceMutex=PTHREAD_MUTEX_INITIALIZER;


void readerSubscribe(){
	pthread_mutex_lock(&readTry);
	pthread_mutex_lock(&readersContention);
	readersCount++;
	if(readersCount==1){
		pthread_mutex_lock(&resourceMutex);
	}
	pthread_mutex_unlock(&readersContention);
	pthread_mutex_unlock(&readTry);
}

void* readerUNSubscribe(void* Args){
	int tID;
	tID = (long)Args;

	pthread_mutex_lock(&readersContention);
	readersCount--;
	if(readersCount==0){
		pthread_mutex_unlock(&resourceMutex);
	}
	pthread_mutex_unlock(&readersContention);
	reader_Sts[tID]=0;
	return NULL;
}
int * counters;
void* taskThrdDoWk(void *threadid)
{
	int thID;
	thID = (int)(long)threadid;
	node_t* queueNode=malloc(sizeof(node_t));

	do{
		if(!reader_Sts[thID]){ //first subscribe me self as Reader
			readerSubscribe();
			reader_Sts[thID]=1;
		}

		sem_wait(&prodConsSemphs[thID]);
		if(exitSignal[thID] && SubRoutinesQueues[thID]==NULL){ //if required and there's no more work EXIT
			pthread_exit(NULL);
		}
		//avoids concurrence issues when retrieving a tasks against the process thread.
		pthread_mutex_lock(&prodConsExcl[thID]);
			pop(&SubRoutinesQueues[thID],&queueNode);//retrieve the subroutine
		pthread_mutex_unlock(&prodConsExcl[thID]);

		queueNode->function(queueNode->Args);
		counters[thID]++;
	} while(1);
}


int addSubRoutine(int thdID, void*(*fp)(void*) ,void* Args){

	pthread_mutex_lock(&prodConsExcl[thdID]);
		push(&SubRoutinesQueues[thdID],fp,Args);
	pthread_mutex_unlock(&prodConsExcl[thdID]);
	sem_post(&prodConsSemphs[thdID]);

	return 0;
}


void insertThreads(int numThds, int reset){
	static int currNumThds=0;
	if(reset) currNumThds=0;

	counters=calloc(numThds,sizeof(int));
	if(currNumThds==0){
		//each thread requires a semaphore, a queue, and a mutex.
		SubRoutinesQueues=malloc(numThds*sizeof(queue));
		for(i=0;i<numThds;i++){
			SubRoutinesQueues[i]=newQueue();
		}

		prodConsSemphs=(sem_t*)malloc(numThds*sizeof(sem_t));
		reader_Sts=(int*)malloc(numThds*sizeof(int));
		exitSignal=(int*)malloc(numThds*sizeof(int));
		prodConsExcl=(pthread_mutex_t*)malloc(numThds*sizeof(pthread_mutex_t));

		threads = (pthread_t*)malloc(numThds*sizeof(pthread_t));

		for(i=0;i<numThds;i++){
			sem_init(&prodConsSemphs[i], 0, 0);
			exitSignal[i]=0;
			reader_Sts[i]=0;
			pthread_mutex_init(&prodConsExcl[i],NULL);
		}

		for( i=0; i < numThds; i++ ){
			err = pthread_create(&threads[i], NULL,taskThrdDoWk, (void *)i);
			if (err){
				printf("Error:unable to create thread!!");
				exit(-1);
			}
		}

		currNumThds+=numThds;


	}else{ //currNumThds!=0
		//READERS WRITERS PROTOCOL.
		pthread_mutex_lock(&readTry); //writer initializes the Request

		for(i=0;i<currNumThds;i++){ //request un-subscribe for each thread
			addSubRoutine(i,readerUNSubscribe,(void*)i);
		}
		pthread_mutex_lock(&resourceMutex); //acquire the mutex to write (lock the resource)
		int newNumThds=currNumThds+numThds;

		SubRoutinesQueues=realloc(SubRoutinesQueues,newNumThds*sizeof(queue));
		for(i=currNumThds;i<newNumThds;i++){
			SubRoutinesQueues[i]=newQueue();
		}

		prodConsSemphs=(sem_t*)realloc(prodConsSemphs,newNumThds*sizeof(sem_t));
		exitSignal=(int*)realloc(exitSignal,newNumThds*sizeof(int));
		reader_Sts=(int*)realloc(reader_Sts,newNumThds*sizeof(int));
		prodConsExcl=(pthread_mutex_t*)realloc(prodConsExcl,newNumThds*sizeof(pthread_mutex_t));

		threads = (pthread_t*)realloc(threads,newNumThds*sizeof(pthread_t));

		for(i=currNumThds;i<newNumThds;i++){
			sem_init(&prodConsSemphs[i], 0, 0);
			exitSignal[i]=0;
			reader_Sts[i]=0;
			pthread_mutex_init(&prodConsExcl[i],NULL);
		}

		for(i=currNumThds;i<newNumThds;i++){
			err = pthread_create(&threads[i], NULL,taskThrdDoWk, (void *)i);
			if (err){
				printf("Error:unable to create thread!!");
				exit(-1);
			}
		}

		currNumThds+=numThds;
		l_numTasks=currNumThds; // Is this a good place to update this value?
								// it is protected by readers-writers but seem some isolated.

		pthread_mutex_unlock(&readTry);
		pthread_mutex_unlock(&resourceMutex);
	}//end else currNumThds!=0

}

int finishThreads(int numThds){
	//printf("Finishing local Tasks.\n");

	int tid;

	for(tid=0;tid<numThds;tid++){ //Send the finish signal to each worker thread
		pthread_mutex_lock(&prodConsExcl[tid]);
		exitSignal[tid]=1;
		pthread_mutex_unlock(&prodConsExcl[tid]);
		sem_post(&prodConsSemphs[tid]);
	}

	for(i=0;i<numThds;i++){
		pthread_join(threads[i], NULL);
	}
	for(i=0;i<numThds;i++){
		pthread_mutex_destroy(&prodConsExcl[i]);
		sem_destroy(&prodConsSemphs[i]);
		free(SubRoutinesQueues[i]);
	}

	free(exitSignal);
	free(reader_Sts);
	free(SubRoutinesQueues);
	free(threads);

	return 0;
}


