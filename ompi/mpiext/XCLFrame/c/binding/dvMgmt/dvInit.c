#include "dvMgmt.h"



//NOTICE: this lib was separated to break apart OpenCL and MPI issues.
//Device management libs//
int g_numDevs; //global variable declared in dvMgmt.h

void initializeDevices(CLxplorInfo*  xplorInfo){

	void *dlhandle;
			void (*devInit)(CLxplorInfo* );
			char *error;


			dlhandle = dlopen ("libmultiDeviceMgmt.so", RTLD_LAZY);
		        if (!dlhandle) {
		            fputs (dlerror(), stderr);
		            exit(1);
		        }


			devInit = dlsym(dlhandle, "devicesInitialization");
		        if ((error = dlerror()) != NULL)  {
		            fputs(error, stderr);
		            exit(1);
		        }

		        (*devInit)(xplorInfo);

		        dlclose(dlhandle);
}
