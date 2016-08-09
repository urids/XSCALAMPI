/*
 * wrapperFactory.h
 *
 *  Created on: Jul 23, 2016
 *      Author: uriel
 */

#ifndef WRAPPERFACTORY_H_
#define WRAPPERFACTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Scheduler Scheduler;
Scheduler* newScheduler(char* heuristicModel);
int matchMake(Scheduler* myScheduler, int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);

#ifdef __cplusplus
}
#endif /* END of __cplusplus*/



#endif /* WRAPPERFACTORY_H_ */
