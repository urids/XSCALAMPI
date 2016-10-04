
#include "scheduling.h"

int __attribute__((visibility("hidden"))) lookFile(FILE** FP, char* fileName){
	*FP=fopen(fileName,"r");
	if(*FP!=NULL)
		return 1;
	return 0;
}

void unitVector(int size, float* unitVector){
	int i;
	for(i=0;i<size;i++){
		unitVector[i]=1.0f;
	}
}

void mtxMult(float *A, float* B, float* C, int M, int K, int N){

	int i,j,k;
	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			C[N*i+j]=0;
			for(k=0;k<K;k++){
				C[N*i+j]+=A[K*i+k]*B[N*k+j];
			}
		}
	}
}

int build_Wij(float *W, int numTasks, int numDevs, int configInputs,char* storagePath){
	int i,j;
	float * accels=malloc(numDevs*numDevs*sizeof(float));
	float * accels0=malloc(numDevs*sizeof(float));
	FILE* PW_Fh; //Poderations wiigh File Handler
	FILE* Accels_Fh;
	if(configInputs & AUTOTUNE){// (all tasks are equally weighted)

		char accelsFile[1024];
		memcpy(accelsFile,storagePath,sizeof(accelsFile));
		strcat(accelsFile,"/accelsFile.dat");
		Accels_Fh=fopen(accelsFile,"r");
		if(Accels_Fh==NULL){
			perror("accelsFile file not found.!! \n");
			exit(-1);
		}

		int readsz;
		readsz=fread(accels,sizeof(float),numDevs*numDevs,Accels_Fh);


		/*printf("\n accelsFile \n");
		for(i=0;i<numDevs;i++){
			for(j=0;j<numDevs;j++){
				printf(" %f",accels[numDevs*i+j]);
			}
			printf("\n");
		}*/

		memcpy(accels0,accels,numDevs*sizeof(float));

		/*printf("\n accels0 \n");
		for(i=0;i<1;i++){
			for(j=0;j<numDevs;j++){
				printf(" %f",accels0[numDevs*i+j]);
			}
			printf("\n");
		}*/


		float * UV=malloc(numTasks*sizeof(float));
		unitVector(numTasks,UV);

		//Wij=UnityVector x Accels[0].
		mtxMult(UV,accels0,W,numTasks,1,numDevs);
	}
	else{
		if(!lookFile(&PW_Fh,"PWF.txt")){
			if(!lookFile(&Accels_Fh,"Accels.txt")){
				//no autotune, no accels =(.
				//Wij=Unity Matrix
			}
			else{ //no autotune but accels file exists
				/*for(i=0;i<numDevs;i++){
					for(j=0;j<numDevs;j++){
						fscanf(Accels_Fh, "%f", &accels[numDevs * i + j]);
					}
				}*/

				fread(accels,sizeof(float),numDevs*numDevs,Accels_Fh);
			}
		}
		else{ // no autotune but PWF.txt exists.

			float* PW=malloc(numTasks*sizeof(float));

			for(i=0;i<numTasks;i++){
				fscanf(PW_Fh, "%f", &PW[i]);	//from text file.
			}

			if(!lookFile(&Accels_Fh,"Accels.txt")){

			}
			else{
				float* PA=malloc(numDevs*sizeof(float));
				for(i=0;i<numDevs;i++){
					fscanf(Accels_Fh, "%f", &PA[i]);	//from accels text file.
				}
				mtxMult(PW,PA,W,numTasks,1,numDevs);
			}

		}

	}


	return 0;
}


int read_Adj(int * adjMtx, int * numTasks){

	int i,j;
	char cwd[1024];
	char* adjFilePath;
	FILE * adjFile_Fh;
	if (getcwd(cwd, sizeof(cwd)) == NULL){
		perror("getcwd() error");
	}

	adjFilePath=strcat(cwd,"/adjMtx.csv");
	adjFile_Fh=fopen(adjFilePath,"r");
	if(adjFile_Fh==NULL){
		perror ("NO Adj_Matrix file found.");
		exit(-1);
	}

	if(adjMtx==NULL){
		* numTasks=0;
		size_t lineSz;
		getline(NULL,&lineSz,adjFile_Fh);
		char* line=malloc(lineSz*sizeof(char));
		getline(&line,&lineSz,adjFile_Fh);

		char* token = strtok(line, " ");
		while (token) {
		    (*numTasks)++;
		    token = strtok(NULL, " ");
		}

	}
	else{
		for(i=0;i<(*numTasks);i++){
			for(j=0;j<(*numTasks);j++){
				fscanf(adjFile_Fh,"%d",&adjMtx[(*numTasks)*i+j]);
			}
		}
	}
	fclose(adjFile_Fh);

	return 0;
}
