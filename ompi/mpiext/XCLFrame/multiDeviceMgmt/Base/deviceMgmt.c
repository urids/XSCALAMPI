
#include "deviceMgmt.h"

int _OMPI_CollectDevicesInfo(int devSelection, MPI_Comm comm){
//int build_g_PUList(int devSelection, MPI_Comm comm){
	int i,j,k,l;
	int myRank,numRanks;

	int l_numDevs=clXplr.numDevices;

	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm,&numRanks);

	int* DPRKS =(int*)malloc(numRanks*sizeof(int)); //DPRKS-> Devices Per RanK Structure
	MPI_Allgather(&l_numDevs,1,MPI_INT,DPRKS,1,MPI_INT,comm);

	for(g_numDevs=0,i=0;i<numRanks;i++){
		g_numDevs+=DPRKS[i];
	}

	//this section creates the global PUs map structure.
	g_PUList=(PUInfo*)malloc(g_numDevs*sizeof(PUInfo)); //TODO: find the space for deallocation.

	for(i=0;i<g_numDevs;i++){
		g_PUList[i].g_PUIdx=i;
	}


	for(i=0,k=0;i<numRanks;i++){
		for(j=0;j<DPRKS[i];j++){
			g_PUList[k].r_rank=i;
			g_PUList[k].l_PUIdx=j;
			k++;
		}
	}

	int offset=0;
	for(i=0;i<myRank;i++){
		offset+=DPRKS[i];
	}

	for(i=offset;i<offset+DPRKS[myRank];){
		for(l=0;l<clXplr.numCPUS;l++,i++){
			g_PUList[i].mappedDevice=cpu[l];
			g_PUList[i].deviceType=CPU_DEVICES;
		}
		for(l=0;l<clXplr.numGPUS;l++,i++){
			g_PUList[i].mappedDevice=gpu[l];
			g_PUList[i].deviceType=GPU_DEVICES;
		}
		for(l=0;l<clXplr.numACCEL;l++,i++){
			g_PUList[i].mappedDevice=accel[l];
			g_PUList[i].deviceType=ACC_DEVICES;
		}
	}

	return g_numDevs; //global variable defined in c_Interface.c
}
