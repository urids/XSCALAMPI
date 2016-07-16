#include "distLinkedList.h"

/* List of locally allocated elements.
d_container_t **l_containers = NULL;
int l_containers_size  = 0;
int l_containers_count = 0;*/



static const meta_ptr_t nil = { -1, (MPI_Aint) MPI_BOTTOM };

/* Allocate a new distributed linked list element */
MPI_Aint alloc_container(int tagVal,  MPI_Win win) {
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

	MPI_Get_address(container_ptr, &disp);
	return disp;
}



int dstrContLst_push(int tagVal){

		meta_ptr_t new_elem_ptr;
		int success;

		/* Create a new list element and register it with the window */
		int myRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

		new_elem_ptr.rank = myRank;
		new_elem_ptr.disp = alloc_container(tagVal, llist_win);

		/* Append the new node to the list.  This might take multiple attempts if
	           others have already appended and our tail pointer is stale. */
		do {
			meta_ptr_t xplr_elem_ptr = nil;

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

			MPI_Compare_and_swap((void*) &new_elem_ptr.rank, (void*) &nil.rank,
					(void*) &xplr_elem_ptr.rank, MPI_INT, tail_ptr.rank,
					(MPI_Aint) &(((d_container_t*)tail_ptr.disp)->next.rank), llist_win);

			MPI_Win_unlock(tail_ptr.rank, llist_win);
			success = (xplr_elem_ptr.rank == nil.rank);

			if (success) {
				int i, flag;

				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

				MPI_Put(&new_elem_ptr.disp, 1, MPI_AINT, tail_ptr.rank,
						(MPI_Aint) &(((d_container_t*)tail_ptr.disp)->next.disp), 1,
						MPI_AINT, llist_win);

				MPI_Win_unlock(tail_ptr.rank, llist_win);
				tail_ptr = new_elem_ptr;

				/* For implementations that use pt-to-pt messaging, force progress for other threads'
	                   RMA operations. */
	                   for (i = 0; i < NPROBE; i++)
	                	   MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

			} else {
				/* Tail pointer is stale, fetch the displacement.  May take multiple tries
	                   if it is being updated. */
				do {
					MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

					MPI_Get( &xplr_elem_ptr.disp, 1, MPI_AINT, tail_ptr.rank,
							(MPI_Aint) &(((d_container_t*)tail_ptr.disp)->next.disp),
							1, MPI_AINT, llist_win);

					MPI_Win_unlock(tail_ptr.rank, llist_win);
				} while (xplr_elem_ptr.disp == nil.disp);
				tail_ptr = xplr_elem_ptr;
			}
		} while (!success);
	return 0;

}

int find_if(int tagVal){
	/* Traverse the list and verify that all processes inserted exactly the correct
       number of elements. */

		int  have_root = 0;
		tail_ptr = head_ptr;

		/* Walk the list and tally up the number of elements inserted by each rank */
		while (tail_ptr.disp != nil.disp) {
			d_container_t elem;

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

			MPI_Get(&elem, sizeof(d_container_t), MPI_BYTE,
					tail_ptr.rank, tail_ptr.disp, sizeof(d_container_t), MPI_BYTE, llist_win);

			MPI_Win_unlock(tail_ptr.rank, llist_win);

			tail_ptr = elem.next;

			/* This is not the root */
			if (have_root) {
				assert(elem.Tag >= 0 );

				if(elem.Tag==tagVal){
					return 1;
				}

			}

			/* This is the root */
			else {
				assert(elem.Tag == -1);
				have_root = 1;
			}
		}
	return 0;
}

