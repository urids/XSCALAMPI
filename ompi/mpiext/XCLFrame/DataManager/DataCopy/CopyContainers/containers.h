/*
 * containers.h
 *
 *  Created on: Jun 11, 2016
 *      Author: uriel
 */

#ifndef CONTAINERS_H_
#define CONTAINERS_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"
#include "semaphore.h"
#include "../../../DataManager/Base/dataManager.h"

typedef struct container_st{
	int containerTAG;
	int containerSize;
	void* Data;
	struct container_st *next;
}container_t;


typedef container_t* list;
void pushContainer(container_t** list, void* Data, int containerSize, int tgID);
void popContainer(container_t** list, container_t** tmpCpy);
void popTagedContainer(container_t** list, int tgID, container_t** tmpCpy);
container_t* newContainersList();

extern list List;
extern sem_t FULL;
extern ticket_t opTicket;

#endif /* CONTAINERS_H_ */
