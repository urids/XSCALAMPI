
#include "poolManager.h"

void push(node_t** queue, function_t function, void* Args){

	if(*queue==NULL){
		*queue=malloc(sizeof(node_t));
		(*queue)->function=function;
		(*queue)->Args=Args;
		(*queue)->next=NULL;
		return;
	}

	node_t *expl=(*queue);//explorer node

	while(expl->next!=NULL){
		expl=expl->next;
	}

	expl->next=malloc(sizeof(node_t));
	expl->next->function=function;
	expl->next->Args=Args;
	expl->next->next=NULL;

}

void pop(node_t** queue, node_t** tmpCpy){

	if((*queue)->next==NULL){
		memcpy(*tmpCpy,*queue,sizeof(node_t));
		*queue=NULL;
	}
	else{
		node_t *nextN;
		nextN=(*queue)->next;
		memcpy((*tmpCpy),*queue,sizeof(node_t));
		free(*queue);
		*queue=nextN;
	}

}

node_t* newQueue(void){
	queue Queue=NULL;
	return Queue;
}
