

#include "dMutexMgmt.h"


MCS_Mutex global_mtx;

int MCS_Mutex_create(int tail_rank, MPI_Comm comm, MCS_Mutex * hdl_out)
{
    int myRank, nproc;
    MCS_Mutex hdl;

    hdl = malloc(sizeof(struct mcs_mutex_s));
    assert(hdl != NULL);

    MPI_Comm_dup(comm, &hdl->comm);

    MPI_Comm_rank(hdl->comm, &myRank);
    MPI_Comm_size(hdl->comm, &nproc);

    hdl->home_rank = tail_rank;

    MPI_Alloc_mem(3*sizeof(int), MPI_INFO_NULL, &hdl->trackInfo);
    assert(hdl->trackInfo != NULL);

    hdl->trackInfo[0] = -1;
    hdl->trackInfo[1] = -1;
    hdl->trackInfo[2] = -1;

    MPI_Win_create(hdl->trackInfo, 3*sizeof(int), sizeof(int), MPI_INFO_NULL,
                   hdl->comm, &hdl->window);

    MPI_Win_lock_all(0, hdl->window);

    *hdl_out = hdl;
    return MPI_SUCCESS;
}


/** Free an MCS mutex.  Collective on ranks in the communicator used at the
  * time of creation.
  *
  * @param[in] hdl handle to the group that will be freed
  * @return        MPI status
  */
int MCS_Mutex_free(MCS_Mutex * hdl_ptr)
{
    MCS_Mutex hdl = *hdl_ptr;

    MPI_Win_unlock_all(hdl->window);

    MPI_Win_free(&hdl->window);
    MPI_Free_mem(hdl->trackInfo);
    MPI_Comm_free(&hdl->comm);

    free(hdl);
    hdl_ptr = NULL;

    return MPI_SUCCESS;
}
