//this matched produced consumer avoids starvation.
#include "intraDev.h"

int intraDevCpyProducer(void* Args){
	//1.- Unwrap the Args.
	struct Args_matchedProducer_st* MP_Args=(struct Args_matchedProducer_st*)Args;
	int tgID=MP_Args->tgID;
	sem_t *FULL=MP_Args->FULL;
	int l_src_task=MP_Args->l_taskIdx;
	int cpyBuffSz=MP_Args->dataSize;
	int src_trayIdx=MP_Args->trayId;
	ticket_t* ticket=MP_Args->ticket;



	//2.- The task thread makes the calls.
	pthread_mutex_lock(&ticket->mtx);

		//readBuffer(l_src_task,src_trayIdx,cpyBuffSz,tmpDataBuff);
		//printf(" res: %d \n", *(int*)tmpDataBuff);
	pushContainer(&List,NULL,cpyBuffSz,tgID);
	pthread_cond_broadcast(&ticket->cond);
	pthread_mutex_unlock(&ticket->mtx);
	sem_post(FULL);

	return 0;
}

int intraDevCpyConsumer(void* Args){
	//1.- Unwrap the Args.
	struct Args_IntraMatchedConsumer_st* MC_Args=(struct Args_IntraMatchedConsumer_st*)Args;
	int tgID=MC_Args->tgID;
	sem_t *FULL=MC_Args->FULL;
	ticket_t* ticket=MC_Args->ticket;
	int l_src_task=MC_Args->l_src_taskIdx;
	int l_dst_task=MC_Args->l_dst_taskIdx;
	int cpyBuffSz=MC_Args->dataSize;
	int dst_trayIdx=MC_Args->dst_trayId;
	int src_trayIdx=MC_Args->src_trayId;


	//2.- The task thread makes the calls.
	sem_wait(FULL);
	pthread_mutex_lock(&ticket->mtx);

	while(1){

		container_t* recContainer=malloc(sizeof(container_t));

		popTagedContainer(&List,tgID,&recContainer);
		if(recContainer!=NULL){

			//writeBuffer(l_dst_task,dst_trayIdx,cpyBuffSz,recContainer->Data);

			intracpyBuffer(l_src_task, src_trayIdx, l_dst_task, dst_trayIdx, cpyBuffSz);
			pthread_mutex_unlock(&ticket->mtx);
			return 0;
		}
		else{
			printf("not Recov\n");
		sem_post(FULL);
		pthread_cond_wait(&ticket->cond,&ticket->mtx);
		}
	}
	//return 0;
}
