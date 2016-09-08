/***************************************************************************
 *                                                                         *
 *          Open MPI: Open Source High Performance Computing               *
 *                                                                         *
 *                   http://www.open-mpi.org/                              *
 *                                                                         *
 ***************************************************************************/

#include "orterun.h"

#include <unistd.h> //for cwd
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ  1024

int main(int argc, char *argv[])
{

int i;
char szTmp[128];
char benchStoragePath[128];
size_t len=128;
key_t key;
int shmid;
char *shm;

sprintf(szTmp, "/proc/%d/exe", getpid());
//fprintf(stdout, "tmp::: %s\n", szTmp);
int bytes = MIN(readlink(szTmp, benchStoragePath, len), len - 1);
if(bytes >= 0){
    benchStoragePath[bytes] = '\0';

     //We'll name our shared memory segment
     //"5678".    
   
    key = 5678;

   

     //Create the segment.
    shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);

      if (shmid < 0) {
	shmctl(shmid, IPC_RMID, NULL); //try a delete.
 	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0){
        perror("shmget SND");
        exit(1);
	}
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
	
	char *s;
 	s = shm;

    for(i=0;i<bytes;i++){
	*s++=benchStoragePath[i];
    }

   *s = NULL;
   
   if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }


}
	

    return orterun(argc, argv);
 
}

/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
