#include "kernelSetup.h"


int argsParser(MPI_Comm comm, int selTask,int workDim , size_t * globalThreads, size_t* localThreads, const char * fmt, va_list arg) {

	int numParam = 0;
	const char *p;
	int j, k; //index exclusive
	int i;
	int tray;
	double dd;
	float f;
	unsigned u;
	char *s;

	cl_mem* memObj=malloc(sizeof(cl_mem));

  int numConsts=0;

	for(p = fmt; *p != '\0'; p++) {
		if(*p != '%') {
			continue;
		}
		switch(*++p) {
		case 'c':
			i = va_arg (arg, int);
			setKernelArgs(selTask,numParam,sizeof(int),&i);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: int, arg number: %d, value: %d \n",numParam,i);
			break;

		case 'd':
			i = va_arg(arg, int);
			setKernelArgs(selTask,numParam,sizeof(int),&i);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: int, arg number: %d, value: %d \n",numParam,i);
			break;
		case 'f':
			dd = va_arg(arg, double);
			f=(float)dd;
			setKernelArgs(selTask,numParam,sizeof(float),&f);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: float, arg number: %d, value: %lf \n",numParam,f);
			break;

		case 's':
			s = va_arg(arg, char *);
			setKernelArgs(selTask, numParam, sizeof(char*), s);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: string, arg number: %d, value: %s \n",numParam, s);
			break;
		case 'T':
			tray = va_arg(arg, int);
			setKernelmemObj(selTask, numParam, sizeof(cl_mem), tray);
			numParam++;
			numConsts++;
			debug_print("\n Debug: argument type: cl_mem, arg number: %d, value: abstract  \n",numParam);
			break;

		case '%':
			putchar('%');
			break;
		}
	}

	enqueueKernel(l_numTasks,selTask,workDim, globalThreads, localThreads);
  return 0;
}

