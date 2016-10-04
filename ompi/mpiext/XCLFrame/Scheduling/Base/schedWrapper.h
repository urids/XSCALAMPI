/*
 * schedWrapper.h
 *
 *  Created on: Sep 23, 2016
 *      Author: uriel
 */

#ifndef SCHEDWRAPPER_H_
#define SCHEDWRAPPER_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Scheduler Scheduler;
Scheduler* newScheduler(char* heuristicModel, char* benchStoragePath);
int matchMake(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);

#ifdef __cplusplus
}
#endif /* END of __cplusplus*/


#endif /* SCHEDWRAPPER_H_ */
