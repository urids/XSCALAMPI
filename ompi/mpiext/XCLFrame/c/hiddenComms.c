

#include "hiddenComms.h"


int OMPI_XclWriteTray(int g_taskIdx, int trayIdx, int bufferSize,void * hostBuffer, MPI_Comm comm){
	return _OMPI_XclWriteTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
}

int OMPI_XclReadTray(int g_taskIdx, int trayIdx, int bufferSize, void * hostBuffer, MPI_Comm comm){
	return _OMPI_XclReadTray(g_taskIdx, trayIdx, bufferSize, hostBuffer, comm);
}

int OMPI_XclMallocTray(int g_taskIdx, int trayIdx, int bufferSize, MPI_Comm comm){
	return _OMPI_XclMallocTray(g_taskIdx, trayIdx, bufferSize, comm);
}

int OMPI_XclFreeTray(int g_taskIdx, int trayIdx, MPI_Comm comm){
	return _OMPI_XclFreeTray(g_taskIdx, trayIdx, comm);
}


int OMPI_XclSendRecv(int src_task, int src_trayIdx,
		             int dest_task, int dest_trayIdx,
		             int count, MPI_Datatype MPIentityType,
		             MPI_Comm comm){
	return _OMPI_XclSendRecv(src_task, src_trayIdx,
            dest_task, dest_trayIdx,
            count, MPIentityType,
            comm);
}


void rcvXploreInfo(MPI_Comm mcomm){

	int testDt;
	MPI_Recv(&testDt,1,MPI_INT,1,0,mcomm,MPI_STATUS_IGNORE);

	debug_print("Root:: Received data form rank %d is: %d \n ",1,testDt);
}


void sendXploreInfo(MPI_Comm mcomm){

	int devsDt=clXplr.numDevices;
	int i,j;

	//dynamically linked function:
	char dvVendor[128];
				void *dlhandle;
				cl_int (*devquery)( cl_device_id  device ,
						 			cl_device_info  param_name ,
						 			size_t  param_value_size ,
						 			void  *param_value ,
						 			size_t  *param_value_size_ret );

				char *error;

				dlhandle = dlopen ("libOpenCL.so", RTLD_LAZY);
			        if (!dlhandle) {
			            fputs (dlerror(), stderr);
			            exit(1);
			        }


				devquery = dlsym(dlhandle, "clGetDeviceInfo");
			        if ((error = dlerror()) != NULL)  {
			            fputs(error, stderr);
			            exit(1);
			        }

				cl_int resError;

				for(i=0;i<clXplr.platformCount;i++){
					for(j=0;j<1;j++){ //TODO: find out how many devices each platform has.
						resError=(*devquery)(clXplr.devices[i][j], CL_DEVICE_VENDOR,
								                    sizeof(dvVendor), &dvVendor, NULL);

						debug_print("Device Vendor: %s \n",dvVendor);


					}
				}

				dlclose(dlhandle);

	MPI_Send(&devsDt,1,MPI_INT,0,0,mcomm);

}



