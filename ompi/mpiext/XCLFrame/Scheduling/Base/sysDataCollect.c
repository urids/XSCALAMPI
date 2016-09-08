#include "scheduling.h"

int runAccelerationTest(char* storagePath){

	void *dlhandle;
	void(*accelerationTest)(char* storagePath);

	char *error;

	dlhandle =dlopen("libbenchmark.so",RTLD_LAZY);
	if (!dlhandle) {
		fputs(dlerror(), stderr);
		exit(1);
	}

	accelerationTest = dlsym(dlhandle, "accelerationTest");

	if ((error = dlerror()) != NULL ) {
		fputs(error, stderr);
		exit(1);
	}
	(*accelerationTest)(storagePath);

	return 0;


}

int runCommsTest(char* storagePath){
	void *dlhandle;
		void(*commsTest)(char* storagePath);

		char *error;

		dlhandle =dlopen("libbenchmark.so",RTLD_LAZY);
		if (!dlhandle) {
			fputs(dlerror(), stderr);
			exit(1);
		}

		commsTest = dlsym(dlhandle, "commsTest");

		if ((error = dlerror()) != NULL ) {
			fputs(error, stderr);
			exit(1);
		}
		(*commsTest)(storagePath);

		return 0;
}


/*int commsBenchmark(commsInfo* cmInf){
	return 0;
}*/




