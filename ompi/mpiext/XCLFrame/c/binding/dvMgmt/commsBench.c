/*
 * This file will perform communication benchmarking
 * between each processing unit (PU) in the system.
 * and is executed
 *
 * */

//call from XCLFame_early just after devices are initialized.

#include "commsBench.h"


device_Task_Info* taskDevMap;
taskInfo* g_taskList; //Global Variable declared at taskMap.h
int l_numTasks;//Global variable declared in tskMgmt.h
int i,j,k;

int commsBenchmark(commsInfo* cmInf){


	int numRanks, myRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

	//*(cmInf->BW_Mtx)=0.0;

	int g_PUs=OMPI_CollectDevicesInfo(ALL_DEVICES, MPI_COMM_WORLD);
	int l_PUs=clXplr.numDevices;

	int* PU_RK =(int*)malloc(numRanks*sizeof(int)); //PU per RanK Structure
	MPI_Allgather(&l_PUs,1,MPI_INT,PU_RK,1,MPI_INT,MPI_COMM_WORLD);

	//this section creates the global PUs map.


	/*	g_PUList=malloc(sizeof(PUInfo)*g_PUs);
	for(i=0;i<g_PUs;i++)
		g_PUList[i].g_PUIdx=i;
	for(i=0,k=0;i<numRanks;i++){
		for(j=0;j<PU_RK[i];j++){
			g_PUList[k].r_rank=i;
			g_PUList[k].l_PUIdx=j;
			k++;
		}
	}*/

	int mygPU_min=0,mygPU_max=0;

	for(i=0;i<myRank;i++){
		mygPU_min+=PU_RK[i];
	}
	mygPU_max=mygPU_min+PU_RK[myRank]-1;


	//printf("g_PUs=%d, l_PUs=%d \n",g_PUs,l_PUs);
	//printf("mygPU_min: %d, mygPU_max: %d \n",mygPU_min, mygPU_max);

	//initialize the local task management system
	l_numTasks=l_PUs;
	taskDevMap=malloc(l_PUs*sizeof(device_Task_Info));
	int slot=0;
	for(i=0;i<clXplr.numCPUS;i++){
		taskDevMap[slot].mappedDevice=cpu[i];
		taskDevMap[slot].min_tskIdx=slot;
		taskDevMap[slot].max_tskIdx=slot;
		slot++;
	}
	for(i=0;i<clXplr.numGPUS;i++){
		taskDevMap[slot].mappedDevice=gpu[i];
		taskDevMap[slot].min_tskIdx=slot;
		taskDevMap[slot].max_tskIdx=slot;
		slot++;
	}
	for(i=0;i<clXplr.numACCEL;i++){
		taskDevMap[slot].mappedDevice=accel[i];
		taskDevMap[slot].min_tskIdx=slot;
		taskDevMap[slot].max_tskIdx=slot;
		slot++;
	}


	//here we allocate space for a provisional local taskList.
	taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_PUs);


	//Here we fill the local taskList and create the memory Racks.
	for (i = 0; i <l_PUs; i++) {
		for (j = taskDevMap[i].min_tskIdx; j <= taskDevMap[i].max_tskIdx;j++) {
			debug_print("-----matching task %d ------\n",j);

			taskList[j].device = taskDevMap[i].mappedDevice;
			taskList[j].numTrays = 0;

			//here we query how many racks has this device.
			int rackIdx = taskList[j].device[0].numRacks;
			if (rackIdx == 0) {
				taskList[j].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				//taskList[j].device[0].memHandler[0] = malloc(1 * sizeof(cl_mem));
				taskList[j].Rack = rackIdx; //rack assignment
				taskList[j].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(taskList[j].device[0].memHandler,(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					taskList[j].Rack = rackIdx;
					taskList[j].device[0].memHandler = tmpRack;
					taskList[j].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d",	__FILE__, __LINE__);
				}
			}
		}
	}



	int* RKS =(int*)malloc(numRanks*sizeof(int)); //RKS-> RanK Structure
	MPI_Allgather(&l_numTasks,1,MPI_INT,RKS,1,MPI_INT,MPI_COMM_WORLD);

	int g_numTasks;
	for(g_numTasks=0,i=0;i<numRanks;i++){
		g_numTasks+=RKS[i];
	}

	g_taskList=(taskInfo*)malloc(g_numTasks*sizeof(taskInfo)); //TODO: find the space for deallocation.

	for(i=0;i<g_numTasks;i++)
		g_taskList[i].g_taskIdx=i;


	//this section creates the global taskList map (temporarily for benchmarking).
	for(i=0,k=0;i<numRanks;i++){
		for(j=0;j<RKS[i];j++){
			g_taskList[k].r_rank=i;
			g_taskList[k].l_taskIdx=j;
			k++;
		}
	}

	//end of task management system initialization

	char* buffer=malloc(sizeof(char)*MAX_SIZE);
	for(i=0;i<MAX_SIZE;i++){
		buffer[i]='X';
	}


	//int numTasks=OMPI_CollectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);

	L_Mtx=malloc(sizeof(double)*g_PUs*g_PUs);
	BW_Mtx=malloc(sizeof(double)*g_PUs*g_PUs);
	(cmInf->BW_Mtx)=BW_Mtx;

	for(i=0;i<g_PUs;i++){
		err|=OMPI_XclWriteTaskBuffer(i, 0, sizeof(char)*MAX_SIZE, buffer, MPI_COMM_WORLD); //first task inits token to 1
		err|=OMPI_XclMallocTaskBuffer(i, 1, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);//remaining tasks allocate space

	}
/*
	if(myRank==1){
		char* testRes=malloc(sizeof(char));
		err |= OMPI_XclReadTaskBuffer(2, 0, sizeof(char), testRes, MPI_COMM_WORLD);
		printf("%c data %c\n",buffer[0],testRes[0]);
	}
*/


	MPI_Win ltcWin,bdwWin;

	if (myRank == ROOT) {
		MPI_Win_create(L_Mtx, sizeof(float)*g_PUs*g_PUs, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &ltcWin);
		MPI_Win_create(BW_Mtx, sizeof(float)*g_PUs*g_PUs, 1, MPI_INFO_NULL,
						MPI_COMM_WORLD, &bdwWin);
	}
	else {
		MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL,
				MPI_COMM_WORLD, &ltcWin);
		MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL,
						MPI_COMM_WORLD, &bdwWin);
	}


	//Latency test

//	float ltc=0;
	 for(src=0;src<g_PUs;src++){
	 		for(dst=0;dst<=src;dst++){
	 			accumTime=0;
	 			for(i=0;i<LATENCY_REPS;i++){
	 				deltaT = 0;
	 				T1 = MPI_Wtime(); // start time
	 				err |= OMPI_XclSendRecv(src, 0, dst, 1, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND
	 				err |= OMPI_XclSendRecv(dst, 1, src, 0, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
	 				T2 = MPI_Wtime(); // end time
	 				deltaT = T2-T1;
	 				accumTime += deltaT;
	 			}
	 			L_Mtx[g_PUs*src+dst]=L_Mtx[src+g_PUs*dst]=(accumTime/LATENCY_REPS); //Symmetric mtx.
	 		}
	 	}

	 MPI_Win_fence(0, ltcWin);
	 //fill the latency matrix at ROOT
	 for(src=mygPU_min;src<g_PUs;src++){
	 	 		for(dst=mygPU_min;dst<=mygPU_max && dst<=src;dst++){
		 	 		MPI_Put(&L_Mtx[g_PUs*src+dst],1,MPI_FLOAT,ROOT,(g_PUs*src+dst)*(sizeof(float)),1,MPI_FLOAT,ltcWin);
		 	 		MPI_Put(&L_Mtx[src+g_PUs*dst],1,MPI_FLOAT,ROOT,(src+g_PUs*dst)*(sizeof(float)),1,MPI_FLOAT,ltcWin);
	 	 		}
	 }

	 MPI_Win_fence(0, ltcWin);

	 //Bandwidth test.


//	 float bwd=0;
		for(src=0;src<g_PUs;src++){
			for(dst=0;dst<=src;dst++){
				msgSz=MIN_SIZE;
				numBWTrials=0;
				sumAvgs=0;
				while(msgSz<=MAX_SIZE){
					deltaT = 0;
					T1 = MPI_Wtime(); // start time
					err |= OMPI_XclSendRecv(src, 0, dst, 1, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND
					err |= OMPI_XclSendRecv(dst, 1, src, 0, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
					T2 = MPI_Wtime(); // end time
					deltaT = T2-T1;
					Avg[numBWTrials]=2*msgSz/deltaT;
					numBWTrials++;
					msgSz<<=1;
				}

				for(i=0;i<numBWTrials;i++){
					sumAvgs+=Avg[i];
				}
				BW_Mtx[g_PUs*src+dst]=BW_Mtx[src+g_PUs*dst]=(sumAvgs/numBWTrials); //the matrix is symmetric.

			}
		}

		MPI_Win_fence(0, bdwWin);

		//fill the bandwidth matrix at ROOT__
		for(src=mygPU_min;src<g_PUs;src++){
			for(dst=mygPU_min;dst<=mygPU_max && dst<=src;dst++){
				MPI_Put(&BW_Mtx[g_PUs*src+dst],1,MPI_FLOAT,ROOT,(g_PUs*src+dst)*(sizeof(float)),1,MPI_FLOAT,bdwWin);
				MPI_Put(&BW_Mtx[src+g_PUs*dst],1,MPI_FLOAT,ROOT,(src+g_PUs*dst)*(sizeof(float)),1,MPI_FLOAT,bdwWin);
			}
		}
		MPI_Win_fence(0, bdwWin);


	//latency test.

/*	for(src=0;src<g_PUs;src++){
		for(dst=0;dst<=src;dst++){
			accumTime=0;
			for(i=0;i<LATENCY_REPS;i++){

				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				err |= OMPI_XclSendRecv(src, 0, dst, 1, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND
				err |= OMPI_XclSendRecv(dst, 1, src, 0, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
				T2 = MPI_Wtime(); // end time
				deltaT = T2-T1;
				accumTime += deltaT;
			}
			L_Mtx[g_PUs*src+dst]=L_Mtx[src+g_PUs*dst]=(accumTime/LATENCY_REPS); //Symmetric mtx.
		}
	}
*/

	//Bandwidth test.
/*
	for(src=0;src<g_PUs;src++){
		for(dst=0;dst<=src;dst++){
			msgSz=MIN_SIZE;
			numBWTrials=0;
			sumAvgs=0;
			while(msgSz<=MAX_SIZE){
				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				err |= OMPI_XclSendRecv(src, 0, dst, 1, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND
				err |= OMPI_XclSendRecv(dst, 1, src, 0, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
				T2 = MPI_Wtime(); // end time
				//printf("src: %d-dst: %d ",src,dst);
				//printf("%f-%f  %f \n",T2,T1,T2-T1);
				deltaT = T2-T1;
				Avg[numBWTrials]=2*msgSz/deltaT;
				numBWTrials++;
				msgSz<<=1;
			}

			for(i=0;i<numBWTrials;i++){
				sumAvgs+=Avg[i];
			}
			BW_Mtx[g_PUs*src+dst]=BW_Mtx[src+g_PUs*dst]=(sumAvgs/numBWTrials); //the matrix is symmetric.
		}
	}

*/
	//Average computations

if(myRank==ROOT){
	float LAccum=0;
	float BWAccum=0;
	for(i=0;i<g_PUs;i++){
		for(j=0;j<g_PUs;j++){
			LAccum+=L_Mtx[g_PUs*i+j];
			BWAccum+=BW_Mtx[g_PUs*i+j];
		}
	}
	L=LAccum/(g_PUs*(g_PUs+1)/2);
	BW=BWAccum/(g_PUs*(g_PUs+1)/2);

	printf("Average Latency: %8.8f microSeconds\n",L*1000000);
	printf("Average Bandwidth: %8.8f GB/s\n",BW/(float)(ONEGB));

	//print matrices

	for(i=0;i<g_PUs;i++){
		printf("| ");
		for(j=0;j<g_PUs;j++){
			printf(" %8.8f |",L_Mtx[g_PUs*i+j]*1000000);
		}
		printf("\n-------------------------------\n");
	}

	printf("\n\n");

	for(i=0;i<g_PUs;i++){
		printf("| ");
		for(j=0;j<g_PUs;j++){
			printf("  %8.6f |",BW_Mtx[g_PUs*i+j]/(float)(ONEGB));
		}
		printf("\n-------------------------------\n");
	}

}


	for(i=0;i<l_PUs;i++){
		OMPI_XclFreeTaskBuffer(i, 0, MPI_COMM_WORLD);
		OMPI_XclFreeTaskBuffer(i, 1, MPI_COMM_WORLD);
	}

	free(buffer);
	for (i = 0; i <l_PUs; i++) {
			for (j = taskDevMap[i].min_tskIdx; j <= taskDevMap[i].max_tskIdx;j++) {
				debug_print("-----matching task %d ------\n",j);
				taskList[j].numTrays = 0;
			}
	}

	//TODO: must we deallocate the number of racks created for each device??
	free(taskDevMap);
	taskDevMap=NULL;
	free(taskList);
	taskList=NULL;
	free(g_taskList);
	g_taskList=NULL;

	return 0;
}
