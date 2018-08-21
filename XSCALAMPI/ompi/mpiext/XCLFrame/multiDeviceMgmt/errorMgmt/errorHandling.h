/*
 * errorHandling.h
 *
 *  Created on: May 3, 2014
 *      Author: uriel
 */

#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#include <stdio.h>
#include "CL/cl.h"

/*inline*/ void chkerr(cl_int status,const char * errName,const char * errfile , const int errline );

#endif /* ERROR_HANDLING_H_ */
