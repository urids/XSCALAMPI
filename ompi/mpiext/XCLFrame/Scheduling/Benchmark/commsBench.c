/*
 * This file will perform communication benchmarking
 * between each processing unit (PU) in the system.
 * and is executed
 *
 * */

//call from XCLFame_early just after devices are initialized.

#include "commsBench.h"
#define VERBOSE 1
/*--device_Task_Info* taskDevMap;
taskInfo* g_taskList; //Global Variable declared at taskManager.h
--*/
//int l_numTasks;//Global variable declared in tskMgmt.h



int commsTest(char* storagePath){

	int err;

	int numRanks, myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);


	char * myBuff;

	int g_NumPUs=_OMPI_CollectDevicesInfo(ALL_DEVICES, MPI_COMM_WORLD);
	printf("NumPUs: %d \n",g_NumPUs);



	int l_PUs=clXplr.numDevices;

	taskDevList=malloc(g_NumPUs*sizeof(schedConfigInfo_t));

	for(i=0;i<g_NumPUs;i++){
		taskDevList[i].g_taskId=i;
		taskDevList[i].rank=g_PUList[i].r_rank;

		//if(g_PUList[i].r_rank==myRank){
			taskDevList[i].mappedDevice=g_PUList[i].mappedDevice;
		//}
	}

	/*for(i=0;i<g_NumPUs;i++){
		printf(" %d | ", taskDevList[i].g_taskId);
		printf(" %d | ", taskDevList[i].rank);
		printf("\n");
	}*/

	g_numTasks=g_NumPUs;
	l_numTasks=l_PUs;


	fillLocalTaskList(MPI_COMM_WORLD);
	fillGlobalTaskList(MPI_COMM_WORLD);
	insertThreads(l_numTasks,0);


	char* buffer=malloc(sizeof(char)*MAX_SIZE);
	for(i=0;i<MAX_SIZE;i++){
		buffer[i]='X';
	}


	/*OMPI_XclMallocTray(0, 0, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);
	OMPI_XclMallocTray(0, 1, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);
	OMPI_XclMallocTray(1, 0, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);
	OMPI_XclMallocTray(1, 1, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);*/

	for(i=0;i<g_numTasks;i++){
		OMPI_XclMallocTray(i, 0, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);
		OMPI_XclMallocTray(i, 1, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);
	}

	OMPI_XclWriteTray( 0, 0, sizeof(char)*MAX_SIZE, buffer, MPI_COMM_WORLD);
	OMPI_XclWriteTray( 1, 0, sizeof(char)*MAX_SIZE, buffer, MPI_COMM_WORLD);

//missing allocations for more tasks.

	L_Mtx=malloc(sizeof(double)*g_NumPUs*g_NumPUs);
	BW_Mtx=malloc(sizeof(double)*g_NumPUs*g_NumPUs);
	MPI_Win bdwWin;
	MPI_Win ltcWin;


	if (myRank == ROOT) {
		MPI_Win_create(L_Mtx, sizeof(float)*g_NumPUs*g_NumPUs, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &ltcWin);
		MPI_Win_create(BW_Mtx, sizeof(float)*g_NumPUs*g_NumPUs, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &bdwWin);
	}
	else {
		MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &ltcWin);
		MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &bdwWin);
	}



	int i;

	//Latency Test
	for(src=0;src<g_NumPUs;src++){
		for(dst=0;dst<=src;dst++){ // must be symmetric.
			accumTime=0;
			for(i=0;i<LATENCY_REPS;i++){
				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				OMPI_XclSendRecv(src, 0, dst, 1, sizeof(char), g_NumPUs*src+dst+i);
				OMPI_XclWaitFor(1, &dst, MPI_COMM_WORLD);
				err |= OMPI_XclSendRecv(dst, 1, src, 0, sizeof(char),g_NumPUs*src+dst+i+1 ); //SEND-BACK
				OMPI_XclWaitFor(1, &src, MPI_COMM_WORLD);
				T2 = MPI_Wtime(); // end time
				deltaT = T2-T1;
				accumTime += deltaT;
			}
			L_Mtx[g_NumPUs*src+dst]=L_Mtx[src+g_NumPUs*dst]=(accumTime/LATENCY_REPS); //Symmetric mtx.
		}
	}

	MPI_Win_fence(0, ltcWin);

	//fill the latency matrix at ROOT
	//(this avoids having zero latency in remote transfers)
	//for(src=mygPU_min;src<g_NumPUs;src++){
		//for(dst=mygPU_min;dst<=mygPU_max && dst<=src;dst++){
			//MPI_Put(&L_Mtx[g_NumPUs*src+dst],1,MPI_FLOAT,ROOT,(g_NumPUs*src+dst)*(sizeof(float)),1,MPI_FLOAT,ltcWin);
			//MPI_Put(&L_Mtx[src+g_NumPUs*dst],1,MPI_FLOAT,ROOT,(src+g_NumPUs*dst)*(sizeof(float)),1,MPI_FLOAT,ltcWin);
		//}
	//}

	MPI_Win_fence(0, ltcWin);

	//write results in persistent storage.
	FILE* L_FH;
	char L_File[1024];
	memcpy(L_File,storagePath,sizeof(L_File));
	strcat(L_File,"/L_File.dat");
	L_FH=fopen(L_File,"w");
	if(L_FH==NULL){
		perror("unable to open latency file");
	}
	fwrite(L_Mtx,sizeof(double),g_NumPUs*g_NumPUs,L_FH);
	fseek(L_FH, SEEK_SET, 0);

	//print them if requested.
#ifdef VERBOSE
	if(myRank==ROOT){
	for(i=0;i<g_NumPUs;i++){
		printf("| ");
		for(j=0;j<g_NumPUs;j++){
			printf(" %8.8f |",L_Mtx[g_NumPUs*i+j]*1000000); //printed in microseconds
		}
		printf("\n-------------------------------\n");
	}
	}
#endif
	OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

	//Bandwidth test.

	for(src=0;src<g_NumPUs;src++){
		for(dst=0;dst<=src;dst++){
			msgSz=MIN_SIZE;
			numBWTrials=0;
			sumAvgs=0;
			while(msgSz<=MAX_SIZE){
				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				OMPI_XclSendRecv(src, 0, dst, 1, sizeof(char)*msgSz, (2*src)+1);
				//OMPI_XclWaitFor(1, &dst, MPI_COMM_WORLD);
				err |= OMPI_XclSendRecv(dst, 1, src, 0, sizeof(char)*msgSz,(2*src)+2 ); //SEND-BACK
				//OMPI_XclWaitFor(1, &src, MPI_COMM_WORLD);
				OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
				T2 = MPI_Wtime(); // end time
				deltaT = T2-T1;
				Avg[numBWTrials]=2*msgSz/deltaT;
				numBWTrials++;
				msgSz<<=1;
			}

			for(i=0;i<numBWTrials;i++){
				sumAvgs+=Avg[i];
			}
			BW_Mtx[g_NumPUs*src+dst]=BW_Mtx[src+g_NumPUs*dst]=(sumAvgs/numBWTrials); //the matrix is symmetric.
		}
	}

	//write results to persistent storage.
	FILE* BW_FH;
	char BW_File[1024];
	memcpy(BW_File,storagePath,sizeof(L_File));
	strcat(BW_File,"/BW_File.dat");
	BW_FH=fopen(BW_File,"w");
	if(BW_FH==NULL){
		perror("unable to open BandWidth file");
	}
	fwrite(BW_Mtx,sizeof(double),g_NumPUs*g_NumPUs,BW_FH);
	fseek(BW_FH, SEEK_SET, 0);
	//print if requested
#ifdef VERBOSE
	if(myRank==ROOT){
	for(i=0;i<g_NumPUs;i++){
		printf("| ");
		for(j=0;j<g_NumPUs;j++){
			printf("  %8.6f |",BW_Mtx[g_NumPUs*i+j]/(float)(ONEGB));
		}
		printf("\n-------------------------------\n");
	}
	}
#endif

	MPI_Win_fence(0, bdwWin);



	finishThreads(l_numTasks);
	free(L_Mtx);
	free(BW_Mtx);
	taskDevList=NULL;
	l_taskList=NULL;
	g_taskList=NULL;
	g_numTasks=0;
	l_numTasks=0;


	return 0;
}
