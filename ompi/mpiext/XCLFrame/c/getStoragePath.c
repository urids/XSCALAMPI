#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>



#define SHMSZ 1024

void getStoragePath(char* path){
	int shmid;
	key_t key;
	char *shm, *s;
	char* net_path=path;

	 // We need to get the segment named
	 // "5678", created by the server.
	key = 5678;
	 // Locate the segment.
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) { // if ! shared Mem
		printf("WARNING! Not shared memory segment at reading Step \n "
				"I will use the default persistent storage Dir instead. \n");

		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL){
		strcat(cwd,"/autoTune\0");
		memcpy(path,cwd,sizeof(cwd));
		}

	} else{ //Successful shmem segment found.

		// attach the segment to our data space.
		if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
			perror("shmat failure.");
			exit(1);
		}

		s=shm;
		while(*path++ =*s++); // Read and copy what the server put in the memory.
		s=NULL;
		while(*--path!='/'); //return until the last slash (to remove orterun)
		memcpy(path,"/autoTune\0",10);
	}


	//create the directory.
	printf("persistetStorageDir:%s \n",net_path);
	struct stat st = {0};
	if (stat(net_path, &st) == -1) {
	    mkdir(net_path, 0775);
	}

}
