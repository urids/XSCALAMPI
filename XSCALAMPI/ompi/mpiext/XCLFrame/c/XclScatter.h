/*
 * XclScatter.h
 *
 *  Created on: Mar 2, 2015
 *      Author: uriel
 */

#ifndef XCLSCATTER_H_
#define XCLSCATTER_H_
#include "stdlib.h"
#include "stdio.h"
#include "dlfcn.h"

#include "ompi/include/mpi.h"
#include "binding/tskMgmt/tskMgmt.h"


struct entityData_st{
	int blockCount;
	int* blocklen;
	MPI_Aint* displacements;
	MPI_Datatype* basictypes;
	MPI_Datatype * newDatatype;
}entityData;

int MPIentityTypeSize;
int entitiesPerRank;

int OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype);


#endif /* XCLSCATTER_H_ */
