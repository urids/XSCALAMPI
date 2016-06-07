#include "dvMgmt.h"

//NOTICE: this binding lib was separated to break apart OpenCL and MPI issues.


void deviceXploration(CLxplorInfo * clXplr){

		void *dlhandle;
		CLxplorInfo (*devXploration)();
		char *error;

		dlhandle = dlopen ("libmultiDeviceMgmt.so", RTLD_LAZY);
	        if (!dlhandle) {
	            fputs (dlerror(), stderr);
	            exit(1);
	        }


		devXploration = dlsym(dlhandle, "deviceExploration");
	        if ((error = dlerror()) != NULL)  {
	            fputs(error, stderr);
	            exit(1);
	        }

	        *clXplr=(*devXploration)();

	       // printf("early: Number of devices available in this node are: %d \n ",clXplr->numDevices);

	        dlclose(dlhandle);

}



