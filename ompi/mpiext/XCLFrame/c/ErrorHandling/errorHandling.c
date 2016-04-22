
#include "errorHandling.h"

/*inline*/ void chkerr(cl_int status,const char * errName,const char * errfile , const int errline ){
        if(status!=CL_SUCCESS){
        	printf("RUNTIME ERROR: \n Event Id => %d \n When: %s " ,status,errName );
        	printf(" AT: %s (%d) \n", errfile,errline);
          exit(EXIT_FAILURE);
        }
}
