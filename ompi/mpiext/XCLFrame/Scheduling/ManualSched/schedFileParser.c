

#include "manualSched.h"
#define MAX_LINE_LEN 256

int getNumDecls(){

	FILE* config_fp;
	char line[MAX_LINE_LEN + 1];
	char* token;
	char cwd[1024];
	int declsLineCounter=0;

	config_fp = fopen("./taskSched.cfg", "r");
	if(config_fp==NULL){

		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printf("Current working dir: %s\n", cwd);
		else
			perror("getcwd() error");

		perror("task-Device configuration file not found.!! \n");
		exit(-1);
	}


	while (fgets(line, MAX_LINE_LEN, config_fp) != NULL) {
		token = strtok(line, "\t =\n\r");
		if (token != NULL && token[0] != '#') {


			if (strcmp(token, "CPU") == 0) declsLineCounter++;
			if (strcmp(token, "GPU") == 0) declsLineCounter++;
			if (strcmp(token, "ACCEL") == 0) declsLineCounter++;


		}
	}

	return declsLineCounter;
}


//int __attribute__ ((visibility ("protected"))) readTaskBinding(device_Task_Info* taskDevMap) {
int readTaskBinding(device_Task_Info* taskDevMap) {
	enum devDesc {
		CPU, GPU, ACCEL
	};

	FILE* config_fp;
	char line[MAX_LINE_LEN + 1];
	char* token;
	char cwd[1024];
	int slot=0;
	int currentMin=0; //min in the taskDev Map

	int devType;

	config_fp = fopen("./taskSched.cfg", "r");
	if(config_fp==NULL){

		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printf("Current working dir: %s\n", cwd);
		else
			perror("getcwd() error");

		perror("task-Device configuration file not found.!! \n");
		exit(-1);
	}

	//parsing of the taskSched.cfg file.
	while (fgets(line, MAX_LINE_LEN, config_fp) != NULL) {
		token = strtok(line, "\t =\n\r");
		if (token != NULL && token[0] != '#') {

			if (strcmp(token, "l_numTasks") == 0) {
				token = strtok(NULL, "\t =\n\r");
				l_numTasks = atoi(token);
				//printf("local num tasks: %d \n", l_numTasks);
			} else {

				if (strcmp(token, "CPU") == 0) devType=0;
				if (strcmp(token, "GPU") == 0) devType=1;
				if (strcmp(token, "ACCEL") == 0) devType=2;


				switch (devType) {
				case CPU:
					//printf("scheduling in %s ",token );
					token = strtok(NULL, "\t =\n\r"); //gets the device Id
					taskDevMap[slot].mappedDevice=cpu[atoi(token)];

					token = strtok(NULL, "\t =\n\r"); //gets the num of tasks
					taskDevMap[slot].min_tskIdx=currentMin;
					taskDevMap[slot].max_tskIdx=currentMin+atoi(token)-1;

					currentMin=currentMin+atoi(token);

					slot++;

					break;
				case GPU:
					/*printf("Device type::\t%s\n", token);
					token = strtok(NULL, "\t =\n\r");
					printf("ID:\t%d\n\n", atoi(token));
					token = strtok(NULL, "\t =\n\r");
					printf("numTasks Assigned:\t%d\n\n", atoi(token));*/


					token = strtok(NULL, "\t =\n\r"); //gets the device Id
					taskDevMap[slot].mappedDevice = gpu[atoi(token)];

					token = strtok(NULL, "\t =\n\r"); //gets the number of tasks
					taskDevMap[slot].min_tskIdx = currentMin;
					taskDevMap[slot].max_tskIdx = currentMin + atoi(token)-1;
					currentMin = currentMin + atoi(token) ;
					slot++;

					break;

				case ACCEL:/*
					//printf("Device type::\t%s\n", token);
					token = strtok(NULL, "\t =\n\r");
					//printf("ID:\t%d\n\n", atoi(token));
					token = strtok(NULL, "\t =\n\r");
					//printf("numTasks Assigned:\t%d\n\n", atoi(token));*/

					token = strtok(NULL, "\t =\n\r"); //reads the device Id
					taskDevMap[slot].mappedDevice = accel[atoi(token)];

					token = strtok(NULL, "\t =\n\r"); //reads the num of tasks
					taskDevMap[slot].min_tskIdx = currentMin;
					taskDevMap[slot].max_tskIdx = currentMin + atoi(token)-1;
					currentMin = currentMin + atoi(token) ;
					slot++;

					break;

				default:
					break;
				}

			}

		}
	}

	if(currentMin!=l_numTasks){
		perror("WARNING:: Please review your taskSched config file. Tasks do not match \n");
		//exit(-1);
	}

	return 0;
}



//--------------------------------------------------------------------
//TODO: this parser must be done using the LR Grammar parsing.
//returns g_numTasks
int readAndParse() {

	enum devDesc {
		CPU, GPU, ACCEL
	};

	FILE* FP;
	//char line[MAX_LINE_LEN + 1];
	char * line=malloc((MAX_LINE_LEN + 1)*sizeof(char));
	char* token;
	char* IDsToken;
	char cwd[1024];
	int slot=0;
	int currSlot=0;
	static int numSlots=0;
	int i,j,k;
	schedConfigInfo_t* tmp_taskDevList=NULL; //this list is unsorted.

	int currentMin=0; //min in the taskDev Map

	int devType;

	FP = fopen("./scheduleFile.cfg", "r");
	if(FP==NULL){
		perror("task-Device manual mapping file not found.!! \n");
		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printf("Current working dir: %s\n", cwd);
		else
			perror("getcwd() error");
		exit(-1);
	}

	//init parsing of the scheduleFile.cfg file.


	while (fgets(line, MAX_LINE_LEN, FP) != NULL) {
		if (strchr(line,'-')!=NULL){ //Extract first hypen separated task ID (if any).
			token = strtok(line, "-"); //Hopefully this will extract the IDS =) or a comment =(.
			if (token != NULL && token[0] != '#') {
				int ini=atoi(token);
				token=strtok(NULL, " "); //Extract second hypen separated task ID.
				int fini=atoi(token);

				token = strtok(NULL, "\t "); //Extract the rank token.
				int rankToken=atoi(token);

				token = strtok(NULL, "\t "); //Extract the DEVICE TYPE token.

				if (strcmp(token, "CPU") == 0) devType=CPU;
				if (strcmp(token, "GPU") == 0) devType=GPU;
				if (strcmp(token, "ACCEL") == 0) devType=ACCEL;


				token = strtok(NULL, "\t "); //Extract the DEVICE ID token.
				int deviceIDToken=atoi(token);


				k=ini;
				for(i=0;i<=(fini-ini);i++,k++){
					tmp_taskDevList=realloc(tmp_taskDevList,(currSlot+1)*sizeof(schedConfigInfo_t));
					tmp_taskDevList[currSlot].g_taskId=k;
					tmp_taskDevList[currSlot].rank=rankToken;
//TODO: This assignment must be done only in the proper rank. "BUG DETECTED"
					switch (devType) {
					case CPU:
						tmp_taskDevList[currSlot].mappedDevice= cpu[deviceIDToken];
						break;
					case GPU:
						tmp_taskDevList[currSlot].mappedDevice = gpu[deviceIDToken];
						break;
					case ACCEL:
						tmp_taskDevList[currSlot].mappedDevice = accel[deviceIDToken];
						break;
					default:
						break;
					}
					currSlot++;
				}
			}
		}
		else if(strchr(line,',')!=NULL){ //if the line have commas

			int* IdsArray=NULL;
			int IdsArrayLeng=0;

			do{
				token = strtok(line, ","); //This will extract the n-1 firsts comma separated IDs.
				if (token != NULL && token[0] != '#') {
					IdsArray=realloc(IdsArray,(IdsArrayLeng+1)*sizeof(int));
					IdsArray[IdsArrayLeng]=atoi(token);
					line +=strlen(token)+1;
					IdsArrayLeng++;
				}
				else{ //if is a comment line break the lecture.
					perror("please remove commas from commented line =(.");
					exit(-1);
				}
			}while(strchr(line,',')!=NULL);

			token = strtok(line, " "); //This will extract the last comma separated ID.
			IdsArray=realloc(IdsArray,(IdsArrayLeng+1)*sizeof(int));
			IdsArray[IdsArrayLeng]=atoi(token);
			IdsArrayLeng++;


			token = strtok(NULL, "\t "); //Extract the rank token.
			int rankToken=atoi(token);

			token = strtok(NULL, "\t "); //Extract the DEVICE TYPE token.

			if (strcmp(token, "CPU") == 0) devType=CPU;
			if (strcmp(token, "GPU") == 0) devType=GPU;
			if (strcmp(token, "ACCEL") == 0) devType=ACCEL;

			token = strtok(NULL, "\t "); //Extract the DEVICE ID token.
			int deviceIDToken=atoi(token);
			for(i=0;i<IdsArrayLeng;i++){
				tmp_taskDevList=realloc(tmp_taskDevList,(currSlot+1)*sizeof(schedConfigInfo_t));
				tmp_taskDevList[currSlot].g_taskId=IdsArray[i];
				tmp_taskDevList[currSlot].rank=rankToken;
//TODO: This assignment must be done only in the proper rank. "BUG DETECTED"
				switch (devType) {
				case CPU:
					tmp_taskDevList[currSlot].mappedDevice=cpu[deviceIDToken];
					break;
				case GPU:
					tmp_taskDevList[currSlot].mappedDevice = gpu[deviceIDToken];
					break;
				case ACCEL:
					tmp_taskDevList[currSlot].mappedDevice = accel[deviceIDToken];
					break;
				default:
					break;
				}
				currSlot++;
			}
		}//ENDif the line have commas

		else{ //if the line is a single decl
			token = strtok(line, " "); //This will extract the ID.
			if (token != NULL && token[0] != '#') {
				int singletaskID=atoi(token);
				token = strtok(NULL, "\t "); //Extract the rank token.
				int rankToken=atoi(token);
				token = strtok(NULL, "\t "); //Extract the DEVICE TYPE token.

				if (strcmp(token, "CPU") == 0) devType=CPU;
				if (strcmp(token, "GPU") == 0) devType=GPU;
				if (strcmp(token, "ACCEL") == 0) devType=ACCEL;
				token = strtok(NULL, "\t "); //Extract the DEVICE ID token.
				int deviceIDToken=atoi(token);

				tmp_taskDevList=realloc(tmp_taskDevList,(currSlot+1)*sizeof(schedConfigInfo_t));
				tmp_taskDevList[currSlot].g_taskId=singletaskID;
				tmp_taskDevList[currSlot].rank=rankToken;
//TODO: This assignment must be done only in the proper rank. "BUG DETECTED"
					switch (devType) {
					case CPU:
						tmp_taskDevList[currSlot].mappedDevice=cpu[deviceIDToken];
						break;
					case GPU:
						tmp_taskDevList[currSlot].mappedDevice = gpu[deviceIDToken];
						break;
					case ACCEL:
						tmp_taskDevList[currSlot].mappedDevice = accel[deviceIDToken];
						break;
					default:
						break;
					}
					currSlot++;

			}
		}//ENDif the line is a single decl

	} //ENDwhile lines

     //g_numTasks=currSlot; //final counting of the global num tasks.
	int taskCount=currSlot;

     //Sorting the taskDevList (On^2 is not good =/);
     taskDevList=malloc(taskCount*sizeof(schedConfigInfo_t));
     for(i=0;i<taskCount;i++){
    	 for(j=0;j<taskCount;j++){
    		 if(tmp_taskDevList[j].g_taskId==i){
    			 taskDevList[i]=tmp_taskDevList[j];
    			 break;
    		 }
    	 }
     }

     /*for(i=0;i<g_numTasks;i++){ //Print the global task list
    	 printf("%d .. %d   \n",taskDevList[i].g_taskId, taskDevList[i].rank);
     }*/

     //taskDevList=tmp_taskDevList;

	return currSlot;
}



