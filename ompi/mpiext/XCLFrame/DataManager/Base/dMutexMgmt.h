/*
 * dMutexMgmt.h
 *
 *  Created on: Jul 15, 2016
 *      Author: uriel
 */

#ifndef DMUTEXMGMT_H_
#define DMUTEXMGMT_H_

#include "../DataCopy/InterNode/mcs-mutex.h"

int MCS_Mutex_create(int tail_rank, MPI_Comm comm, MCS_Mutex * hdl_out);
int MCS_Mutex_free(MCS_Mutex * hdl_ptr);



#endif /* DMUTEXMGMT_H_ */
