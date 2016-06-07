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
//#include "../../c/binding/tskMgmt/tskMgmt.h"
#include "../../TaskManager/Base/taskManager.h"

#include "../Base/entityInfo.h"

struct entityData_st{
	int blockCount;
	int* blocklen;
	MPI_Aint* displacements;
	MPI_Datatype* basictypes;
	MPI_Datatype * newDatatype;
}entityData;


//this declaration could be in mpiext_XCLFrame_c.h but also can be here because this file
// is included in the automake file TwoSidedComms/makefile.am
int OMPI_commit_EntityType(int blockcount, int* blocklen, MPI_Aint* displacements, MPI_Datatype* basictypes, MPI_Datatype * newDatatype);


#endif /* XCLSCATTER_H_ */
