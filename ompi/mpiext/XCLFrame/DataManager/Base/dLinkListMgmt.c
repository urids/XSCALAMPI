
#include "dLinkListMgmt.h"

static const meta_ptr_t nil = { -1, (MPI_Aint) MPI_BOTTOM };

MPI_Win  llist_win; //Declared at distLinkedList.h
/* List of locally allocated elements.
 * in the distributed linked list */
d_container_t **l_containers;
int l_containers_size;
int l_containers_count;
meta_ptr_t head_ptr, tail_ptr;

static MPI_Aint alloc_container(int tagVal,  MPI_Win win) {
	MPI_Aint disp;
	d_container_t *container_ptr;

	/* Allocate the new element */
	MPI_Alloc_mem(sizeof(d_container_t), MPI_INFO_NULL, &container_ptr);
	container_ptr->Tag = tagVal;
	container_ptr->next  = nil;

	/* then register it within the window */
	MPI_Win_attach(win, container_ptr, sizeof(d_container_t));

	/* Add the element to the l_containers list so we can free it later. */
	if (l_containers_size == l_containers_count) {
		l_containers_size += 100;
		l_containers = realloc(l_containers, l_containers_size*sizeof(void*));
	}
	l_containers[l_containers_count] = container_ptr;
	l_containers_count++;

	MPI_Get_address(container_ptr, &disp);int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	return disp;
}


int new_dstrContLst(int hostRank){
	MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &llist_win);
	int myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

		/* hostRank creates the head node */
		if (myRank == hostRank)
			head_ptr.disp = alloc_container(-1, llist_win);

		/* Broadcast the head pointer to everyone */
		head_ptr.rank = hostRank;
		MPI_Bcast(&head_ptr.disp, 1, MPI_AINT, hostRank, MPI_COMM_WORLD);
		tail_ptr = head_ptr;

	return 0;
}


int free_dstrContLst(){
	MPI_Win_free(&llist_win);
	/* Free all the elements in the list */
	for ( ; l_containers_count > 0; l_containers_count--)
		MPI_Free_mem(l_containers[l_containers_count-1]);

	return 0;
}
