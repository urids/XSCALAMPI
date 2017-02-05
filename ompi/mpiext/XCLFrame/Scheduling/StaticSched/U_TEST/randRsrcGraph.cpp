#include "common.h"
using namespace std;

execEnvironment::execEnvironment(){
	numRanks=0;
	numDevices=0;
	theta=U(5.0,10.0);
	alpha=U(0.2,0.5);
	beta =U(.01,.5);
	gamma=U(.2,5.0);

	build_Nodes();
	build_Connections();
	run_Benchmarks(); //computation Benchmarks
}


void execEnvironment::build_Nodes(){

	int i,j;
	int newDevices=0;
	numRanks = U(MIN_RANKS,MAX_RANKS);
	int devConunt=0;
	for(i=0;i<numRanks;i++){
		int newCPUDevices = U(MIN_CPUS,MAX_CPUS);
		int newGPUDevices = U(MIN_GPUS,MAX_GPUS);

		int newDevices=newCPUDevices+newGPUDevices;

		for(j=0;j<newCPUDevices;j++){
			PU_t tmpCPU;
			tmpCPU.ID=devConunt++;
			tmpCPU.type=CPU;
			tmpCPU.rank=i;
			PUs.push_back(tmpCPU);

		}
		for(j=0;j<newGPUDevices;j++){
			PU_t tmpGPU;
			tmpGPU.ID=devConunt++;
			tmpGPU.type=GPU;
			tmpGPU.rank=i;
			PUs.push_back(tmpGPU);
		}

		numDevices+=newDevices;
	}
	//setmax memory space constraint on each device.
	/*	for_each(PUs.begin(),PUs.end(),[&](PU_t device){
		if(device.type){
			s.push_back(U(MIN_GPU_MEM,MAX_GPU_MEM));
		}else{
			s.push_back(U(MIN_CPU_MEM,MAX_CPU_MEM));
		}
	});*/


	s=(int*)malloc(numDevices*sizeof(int));
	for(i=0;i<numDevices;i++){
		if(PUs[i].type){
			s[i]=(1<<U(MIN_GPU_MEM,MAX_GPU_MEM))*1024;
		}else{
			s[i]=(1<<U(MIN_CPU_MEM,MAX_CPU_MEM))*1024;
		}
		//const char* type = PUs[i].type?"GPU":"CPU";
		//printf("type: %s, s:= %d \n",type , s[i]);
	}

}

//rule flags definition
char dstDevice=0x1; // 0000 0001
char srcDevice=0x2; // 0000 0010
char copyMode=0x4;  // 0000 0100


void execEnvironment::rule4(char flag, int i, int j){
	int rule = (flag & srcDevice)+(flag & dstDevice);
	//printf("rule %d \n",rule);
	if(!(flag & copyMode)){ // interDevice
		switch (rule) {
		//case 0 assume NUMA aprox X.
		case 1:
		case 2: //case 1 and 2 are equal in theory
			BWMtx[numDevices*i+j]=alpha*X;
			LMtx[numDevices*i+j]=Y/alpha;
			break;
		case 3:
			BWMtx[numDevices*i+j]=gamma*X;
			LMtx[numDevices*i+j]=Y/gamma;
			break;
		default:
			break;
		}
	}else{ // interNode

		switch (rule) {
		case 0:
			BWMtx[numDevices*i+j]=beta*X;
			LMtx[numDevices*i+j]=Y/beta;
			break;
		case 1:
		case 2:
		case 3:
			BWMtx[numDevices*i+j]=alpha*beta*X;
			LMtx[numDevices*i+j]=Y/(alpha*beta);
			break;
		default:
			break;
		}
	}
}

void execEnvironment::build_Connections(){
	int i,j;
	BWMtx = (float*)malloc(numDevices*numDevices*sizeof(float));
	LMtx = (float*)malloc(numDevices*numDevices*sizeof(float));

	for(i=0;i<numDevices;i++){
		for(j=0;j<i;j++){
			if(i==j){
				if(!PUs[i].type){
					BWMtx[numDevices*i+j]=X;
					BWMtx[numDevices*i+j]=Y;
				}else{
					BWMtx[numDevices*i+j]=theta*X;
					BWMtx[numDevices*i+j]=Y/theta;
				}
			}

			else{
				char flag=0x0;

				if(PUs[i].rank!=PUs[j].rank)//same rank
					flag|=copyMode; //interNode
				if(PUs[i].type)
					flag|=srcDevice; //src Device is GPU
				if(PUs[j].type)
					flag|=dstDevice; //dst Device is GPU
				rule4(flag,i,j);

			}
		}
	}

	//once built we must store them (simulating stored system benchmarks);


	FILE* L_file;
	FILE* BW_file;
	char simuL_File [1024];
	char simuBW_File [1024];
	memcpy(simuL_File,U_TEST_Path,sizeof(simuL_File));
	memcpy(simuBW_File,U_TEST_Path,sizeof(simuBW_File));

	strcat(simuL_File,"/L_File.dat");
	strcat(simuBW_File,"/BW_File.dat");

	L_file=fopen(simuL_File,"w");
	if(L_file==NULL){
		printf("unable to open simulated latency file");
	}
	fwrite(LMtx,sizeof(float),numDevices*numDevices,L_file);
	fseek(L_file, SEEK_SET, 0);

	BW_file=fopen(simuBW_File,"w");
	if(BW_file==NULL){
		printf("unable to open simulated bandwidth file");
	}
	fwrite(BWMtx,sizeof(float),numDevices*numDevices,BW_file);
	fseek(BW_file, SEEK_SET, 0);

}

void execEnvironment::run_Benchmarks(){
	int i,j;
	double* bench_ExecTime= (double*)malloc(numDevices*sizeof(double));
	for(i=0;i<numDevices;i++){
		if(PUs[i].type){
			bench_ExecTime[i]=U(MIN_GPU_BMARK,MAX_GPU_BMARK);
		}else{
			bench_ExecTime[i]=U(MIN_CPU_BMARK,MAX_CPU_BMARK);
		}
	}

	AccMtx=(float*)malloc(numDevices*numDevices*sizeof(float));
	for(i=0;i<numDevices;i++){
		for(j=0;j<=i;j++){
			AccMtx[numDevices*i+j]=AccMtx[numDevices*j+i]
			                      =(float)(bench_ExecTime[j]/bench_ExecTime[i]);

		}
	}

	for(i=0;i<numDevices;i++){
			for(j=0;j<numDevices;j++){
				printf(" %d ",(int)(100.0*AccMtx[numDevices*i+j]));
			}
			printf(" \n");
	}
}
