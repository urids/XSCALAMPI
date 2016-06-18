
#include "containers.h"
void  __attribute__ ((visibility ("hidden")))
pushContainer(container_t** list, void* Data, int containerSize, int tgID){

	if(*list==NULL){
		*list=malloc(sizeof(container_t));
		//(*list)->Data=malloc(containerSize);
		(*list)->Data=Data;
		(*list)->containerSize=containerSize;
		(*list)->containerTAG=tgID;
		(*list)->next=NULL;
		return;
	}

	container_t *expl=(*list);//explorer node

	while(expl->next!=NULL){
		expl=expl->next;
	}

	expl->next=malloc(sizeof(container_t));
	expl->next->Data=Data;
	expl->next->containerSize=containerSize;
	expl->next->containerTAG=tgID;
	expl->next->next=NULL;

}

void  __attribute__ ((visibility ("hidden")))
popContainer(container_t** list, container_t** tmpCpy){
	if((*list)->next==NULL){
		memcpy(*tmpCpy,*list,sizeof(container_t));
		free(*list);
		*list=NULL;
	}
	else{
		container_t *nextN;
		nextN=(*list)->next;
		memcpy((*tmpCpy),*list,sizeof(container_t));
		free(*list);
		*list=nextN;
	}

}

void  __attribute__ ((visibility ("hidden")))
popTagedContainer(container_t** list, int tgID, container_t** tmpCpy){
	container_t *expl=(*list);//explorer node
	if(*list==NULL){
		*tmpCpy=NULL;
		return;
	}
	if(expl->containerTAG==tgID){
		popContainer(&expl,tmpCpy);
		return;
	}

	while(expl->next!=NULL){
		if(expl->next->containerTAG==tgID){
			popContainer(&expl->next,tmpCpy);
			return;
		}
		else{
			expl=expl->next;
		}
	}

	*tmpCpy=NULL;
}

container_t*  __attribute__ ((visibility ("hidden")))
newContainersList(){
	list List=NULL;
	return List;
}
