

#include "dvMgmt.h"

//void initEntitiesBuffer(int numtasks,int HentitiesbufferSize,int MPIentityTypeSize,cl_mem *memObjects){

void * BIND2MULTIDEV_Handle=NULL;

void ALL_TASK_initBuffer(int numtasks, int trayIdx, int HentitiesbufferSize, int MPIentityTypeSize){


	void (*initEntitiesBuffer)(int,int,int, int trayIdx);
	char *error;

	BIND2MULTIDEV_Handle=dlopen("libmultiDeviceMgmt.so",RTLD_LAZY);

	if(!BIND2MULTIDEV_Handle){
		printf("library not found or could not be opened %d, %d" ,__FILE__, __LINE__);
		exit(1);
	}

	initEntitiesBuffer = dlsym(BIND2MULTIDEV_Handle, "ALL_TASKS_initBuffer");
	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	(*initEntitiesBuffer)(numtasks,trayIdx, HentitiesbufferSize, MPIentityTypeSize);

	//TODO: release the BIND2MULTIDEV_Handle.
}


void ALL_TASK_writeBuffer(int numtasks, int trayIdx, void * entitiesbuffer ){


	void * memWrtHandle=NULL;
	void (*writeEntitiesBuffer)(int,int,void*);
	char *error;

	memWrtHandle=dlopen("libmultiDeviceMgmt.so",RTLD_LAZY);

	if(!memWrtHandle){
		printf("library not found or could not be opened %d, %d" ,__FILE__, __LINE__);
		exit(1);
	}

	writeEntitiesBuffer = dlsym(memWrtHandle, "ALL_TASKS_writeBuffer");
	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	(*writeEntitiesBuffer)(numtasks,trayIdx,entitiesbuffer);
}



void ALL_TASKS_readBuffer(int numtasks,int trayIdx, void * entitiesbuffer){

	void * memReadHandle=NULL;
	void (*readEntitiesBuffer)(int,int,void*);
	char *error;

	memReadHandle=dlopen("libmultiDeviceMgmt.so",RTLD_LAZY);

	if(!memReadHandle){
		printf("library not found or could not be opened %d, %d" ,__FILE__, __LINE__);
		exit(1);
	}

	readEntitiesBuffer = dlsym(memReadHandle, "ALL_TASKS_readBuffer");
	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	(*readEntitiesBuffer)(numtasks,trayIdx,entitiesbuffer);

}


