#include "utilities.h"


int isSource(int i, int* Adj, int N){ //never receives (Column of zeros)
	int j;
	for(j=0;j<N;j++){
		if (Adj[N*j+i]) return 0;
	}
	return 1;
}

int isSink(int i, int* Adj, int N){ //never sends (row of zeros)
	int j;
	for(j=0;j<N;j++){
		if (Adj[N*i+j]) return 0;
	}
	return 1;
}

void buildExtendedAdj(int* EAdj, int* Adj , int N){
	int i,j;
	int xtndSz=N+2;
	for(i=0;i<N;i++){
		EAdj[i+1]=isSource(i,Adj,N);
	}

	for(i=1;i<N+1;i++){
		EAdj[xtndSz*i+(N+1)]=isSink(i-1,Adj,N);
	}

	for(i=0,j=1;i<N*N;i+=N,j++){ //copy one row at time
		memcpy(EAdj+(xtndSz*j+1),Adj+i,N*sizeof(int)); // advance EAdj j-rows of xtnd size.
	}

}

void buildExtendedWij(float* EW, float* W , int numTasks, int numDvs){
	memcpy(EW+numDvs,W,sizeof(float)*numTasks*numDvs);
}

float get_LAvg(double * L_Mtx,int g_NumPUs){
	int i,j;
	double L;
	float LAccum=0;
	for(i=0;i<g_NumPUs;i++){
		for(j=0;j<=i;j++){ //sum triangular inferior of the values due to the other are repeated.
			LAccum+=L_Mtx[g_NumPUs*i+j];
		}
	}
	L=LAccum/(g_NumPUs*(g_NumPUs+1)/2);
#if VERBOSE
	printf("Average Latency: %8.8f microSeconds\n",L*1000000);
#endif
	return L;
}

float get_BWAvg(double* BW_Mtx,int g_NumPUs){
	int i,j;
	double BW;
	float BWAccum=0;
	for(i=0;i<g_NumPUs;i++){
		for(j=0;j<=i;j++){ //sum triangular inferior of the values due to the other are repeated.
			BWAccum+=BW_Mtx[g_NumPUs*i+j];
		}
	}
	BW=BWAccum/(g_NumPUs*(g_NumPUs+1)/2);
#if VERBOSE
	printf("Average Bandwidth: %8.8f GB/s\n",BW/(float)(ONEGB));
#endif
	return BW;
}
