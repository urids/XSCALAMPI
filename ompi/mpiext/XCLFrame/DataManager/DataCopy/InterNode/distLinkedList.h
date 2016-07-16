/*
 * distLinkedList.h
 *
 *  Created on: Jul 14, 2016
 *      Author: uriel
 */

#ifndef DISTLINKEDLIST_H_
#define DISTLINKEDLIST_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "ompi/include/mpi.h"

#define NPROBE  100

/* Linked list pointer */
typedef struct {
	int      rank;
	MPI_Aint disp;
} meta_ptr_t;

typedef struct dcontainer_st{
	int Tag;
	meta_ptr_t next;
}d_container_t;

extern MPI_Win llist_win;
extern d_container_t **l_containers;
extern int l_containers_size;
extern int l_containers_count;
extern meta_ptr_t head_ptr, tail_ptr;

int find_if(int tagVal);

#endif /* DISTLINKEDLIST_H_ */
