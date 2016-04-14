
#include  "tskMgmt.h"
#include "../../../TaskManager/Base/taskManager.h"

#define MAX_LINE_LEN 256

int __attribute__ ((visibility ("protected"))) getNumDecls(){

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


int __attribute__ ((visibility ("protected"))) readTaskBinding(device_Task_Info* taskDevMap) {

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
