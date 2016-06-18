/*
 * entityInfo.h
 *
 *  Created on: Apr 15, 2015
 *      Author: uriel
 */

#ifndef ENTITYINFO_H_
#define ENTITYINFO_H_

extern int MPIentityTypeSize;
extern int ePerRank;

typedef struct entityData_st{
	int blockCount;
	int* blocklen;
	MPI_Aint* displacements;
	MPI_Datatype* basictypes;
	MPI_Datatype * newDatatype;
}entityData_t;

#endif /* ENTITYINFO_H_ */
