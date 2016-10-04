

#include "RR.h"
using namespace std;


RR::RR(){

}

RR::~RR(){

}

int RR::matchMake(int NumTsk, int NumDvs,float * W, int * AdjMtx,const int* SR,const int* AS, int* schedule){
	int i;
	static int currNumTasks=0;

	if(!currNumTasks){

		for(i=0;i<NumTsk;i++){
			schedule[i]=i%NumDvs;
		}

	}else{ //not the first call
		int resize=currNumTasks+NumTsk;
		for(i=currNumTasks;i<resize;i++){
			schedule[i]=i%NumDvs;
		}
	}
	currNumTasks+=NumTsk;
	return 0;
}


