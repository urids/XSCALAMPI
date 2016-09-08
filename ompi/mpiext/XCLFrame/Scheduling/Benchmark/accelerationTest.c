#include "../../multiDeviceMgmt/deviceMgmt/deviceExploration.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

void accelerationTest(char* storagePath){
	int i,j;
	int numDevs=clXplr.numDevices;

	float* accelsMtx=malloc(numDevs*numDevs*sizeof(float));

	for(i=0;i<numDevs;i++){
		for(j=0;j<numDevs;j++){
			if(j){
				accelsMtx[numDevs*i+j]=.10;
			}else{
			accelsMtx[numDevs*i+j]=1.0;
			}
		}
	}

	FILE* accels_FH;
	char accelsFile[1024];
	memcpy(accelsFile,storagePath,sizeof(accelsFile));
	strcat(accelsFile,"/accelsFile.dat");
	accels_FH=fopen(accelsFile,"w");
	if(accels_FH==NULL){
		perror("unable to find accels file");
	}

	fwrite(accelsMtx,sizeof(float),numDevs*numDevs,accels_FH);
	fseek(accels_FH, SEEK_SET, 0);
	//fclose(accels_FH);

}
