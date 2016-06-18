//this matched produced consumer avoids starvation.
#include "interDev.h"

int interDevCpyProducer(void* Args){
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
	void* tmpDataBuff=malloc(cpyBuffSz);

	/*struct Args_ReadTray_st * readTray_Args=malloc(sizeof(struct Args_ReadTray_st));
			readTray_Args->l_taskIdx=l_src_task;
			readTray_Args->trayIdx=src_trayIdx;
			readTray_Args->bufferSize=cpyBuffSz;
			readTray_Args->hostBuffer=tmpDataBuff;
		_OMPI_XclReadTray(readTray_Args);*/

		readBuffer(l_src_task,src_trayIdx,cpyBuffSz,tmpDataBuff);

		pushContainer(&List,tmpDataBuff,cpyBuffSz,tgID);
	pthread_cond_broadcast(&ticket->cond);
	pthread_mutex_unlock(&ticket->mtx);
	sem_post(FULL);
	return 0;
}

int interDevCpyConsumer(void* Args){
	//1.- Unwrap the Args.
	struct Args_matchedConsumer_st* MC_Args=(struct Args_matchedConsumer_st*)Args;
	int tgID=MC_Args->tgID;
	sem_t *FULL=MC_Args->FULL;
	ticket_t* ticket=MC_Args->ticket;
	int l_dst_task=MC_Args->l_taskIdx;
	int cpyBuffSz=MC_Args->dataSize;
	int dst_trayIdx=MC_Args->trayId;

	//2.- The task thread makes the calls.
	sem_wait(FULL);
	pthread_mutex_lock(&ticket->mtx);
	while(1){
		container_t* recContainer=malloc(sizeof(container_t));
		popTagedContainer(&List,tgID,&recContainer);
		if(recContainer!=NULL){
			/*struct Args_WriteTray_st * writeTray_Args=malloc(sizeof(struct Args_WriteTray_st));
				writeTray_Args->l_taskIdx=l_dst_task;
				writeTray_Args->trayIdx=dst_trayIdx;
				writeTray_Args->bufferSize=cpyBuffSz;
				writeTray_Args->hostBuffer=recContainer->Data;

			_OMPI_XclWriteTray(writeTray_Args);*/
			writeBuffer(l_dst_task,dst_trayIdx,cpyBuffSz,recContainer->Data);
			//Freeing the container.
			pthread_mutex_unlock(&ticket->mtx);
			return 0;
		}
		else{
		sem_post(FULL);
		pthread_cond_wait(&ticket->cond,&ticket->mtx);
		}
	}
	//return 0;
}
