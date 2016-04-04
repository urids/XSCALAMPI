
#include "tskMgmt.h"
#define DEBUG 0
//This file performs Task<->Device matching
device_Task_Info* taskDevMap;

int createTaskList(int devSelection){ //There exist only one l_taskList per Node and each task has a device.

int i,j;
//taskDevMap=malloc(clXplr.numDevices*sizeof(device_Task_Info));

int numDecls=getNumDecls(); //this first step enables to get the number of lines with matchmaking decls.
taskDevMap=malloc(numDecls*sizeof(device_Task_Info));

for (i = 0; i < numDecls; i++) {
			taskDevMap[i].min_tskIdx=-1; //By default each device has no assigned task.
}

readTaskBinding(taskDevMap); //function defined in taskScheduling.c to read taskSched.cfg file.

debug_print("1.- read taskDevMap succeed!!\n");


	switch (devSelection) {
	case ALL_DEVICES:
		//here we allocate space for the local l_taskList.
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		//Each task needs a handler (pointer) to its device
		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(1*sizeof(Device)); //each task has only 1 device.
		}

//int advance=0;
		//Here we fill the l_taskList and create the memory Racks.
		//for (i = 0; i <l_numTasks; i++) {
		for (i = 0; i <numDecls; i++) {
			//advance=taskDevMap[i].max_tskIdx-taskDevMap[i].min_tskIdx+1;
			if(taskDevMap[i].min_tskIdx!=(-1))//perform resource allocation iff has any assign.
			for (j = taskDevMap[i].min_tskIdx; j <= taskDevMap[i].max_tskIdx;j++) {
				debug_print("-----matching task %d , %d- %d ------\n",j,taskDevMap[i].min_tskIdx,taskDevMap[i].max_tskIdx);

				l_taskList[j].device = taskDevMap[i].mappedDevice;
				l_taskList[j].numTrays = 0;

				//here we query how many racks has this device.
				int rackIdx = l_taskList[j].device[0].numRacks;
				if (rackIdx == 0) {
					l_taskList[j].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
					//l_taskList[j].device[0].memHandler[0] = malloc(1 * sizeof(cl_mem));
					l_taskList[j].Rack = rackIdx; //rack assignment
					l_taskList[j].device[0].numRacks++;
				} else {
					cl_mem** tmpRack;
					tmpRack = (cl_mem**) realloc(l_taskList[j].device[0].memHandler,(rackIdx + 1) * sizeof(cl_mem*));
					if (tmpRack != NULL) {
						l_taskList[j].Rack = rackIdx;
						l_taskList[j].device[0].memHandler = tmpRack;
						l_taskList[j].device[0].numRacks++;
					} else {
						printf("ERROR AT: Reallocating racks. %d , %d",	__FILE__, __LINE__);
					}
				}
			}

		}

		break;



	case CPU_DEVICES:
		//l_numTasks = 4;
		printf("CPU EXEC\n");
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);


		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = cpu[0];//TODO: switch device number.
			l_taskList[i].numTrays = 0; //init the number of device memBuffers to zero

			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}

			//l_taskList[i].Rack=i;
			//l_taskList[i].device[0].numRacks++;
		}
		break;

	case GPU_DEVICES:
		//l_numTasks = 4;
		printf("GPU EXEC\n");
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = gpu[0]; //TODO: switch device number.
			l_taskList[i].numTrays = 0; //init the number of device memBuffers to zero

			//here we query how many racks has this device.
			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				//l_taskList[i].device[0].memHandler[0]=malloc(1*sizeof(cl_mem));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}
		}

		break;

	case ACC_DEVICES:
		//l_numTasks = clXplr.numACCEL;
		l_taskList = (XCLtask*) malloc(sizeof(XCLtask) * l_numTasks);

		for (i = 0; i < l_numTasks; i++) {
			l_taskList[i].device = (Device*) malloc(sizeof(Device));
			l_taskList[i].device = accel[i];
			l_taskList[i].numTrays=0;//init the number of device memBuffers to zero
			//here we query how many racks has this device.
			int rackIdx = l_taskList[i].device[0].numRacks;
			if (rackIdx == 0) {
				l_taskList[i].device[0].memHandler = malloc(1 * sizeof(cl_mem*));
				//l_taskList[i].device[0].memHandler[0]=malloc(1*sizeof(cl_mem));
				l_taskList[i].Rack = rackIdx;
				l_taskList[i].device[0].numRacks++;
			} else {
				cl_mem** tmpRack;
				tmpRack = (cl_mem**) realloc(l_taskList[i].device[0].memHandler,
						(rackIdx + 1) * sizeof(cl_mem*));
				if (tmpRack != NULL) {
					l_taskList[i].Rack = rackIdx;
					l_taskList[i].device[0].memHandler = tmpRack;
					l_taskList[i].device[0].numRacks++;
				} else {
					printf("ERROR AT: Reallocating racks. %d , %d", __FILE__,
							__LINE__);
				}
			}

		}

		break;

	default:
		break;
	}
return 0;


}


int XclCreateKernel(MPI_Comm comm, int l_selTask, char* srcPath,char* kernelName,int numTasks){


	int i,j,k;
	void *dlhandle;
	int (*createProgram)(int l_selTask, char*,int);
	int (*buildProgram)(int l_selTask, int);
	int (*createKernel)(int l_selTask, char*,int);
	int (*kernelXplor)(int l_selTask, int);

	char *error;

	dlhandle =dlopen("libmultiDeviceMgmt.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	createProgram = dlsym(dlhandle, "createProgram");
	buildProgram = dlsym(dlhandle, "buildProgram");
	createKernel = dlsym(dlhandle, "createKernel");
	kernelXplor = dlsym(dlhandle, "kernelXplor");
	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}

	debug_print("0.-kernelSrcFile: %s\n",srcPath);
		int err;
		err=(*createProgram)(l_selTask, srcPath,numTasks);
		err|=(*buildProgram)(l_selTask, numTasks);

		err|=(*createKernel)(l_selTask, kernelName, numTasks);
		err|=(*kernelXplor)(l_selTask, numTasks);


	dlclose(dlhandle);

return 0;


}
