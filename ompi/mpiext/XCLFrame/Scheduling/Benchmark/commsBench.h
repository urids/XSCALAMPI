/*
 * commsBench.h
 *
 *  Created on: Sep 14, 2015
 *      Author: uriel
 */

#ifndef COMMSBENCH_H_
#define COMMSBENCH_H_


#include "stdlib.h"
#include "stdio.h"

#include "ompi/include/mpi.h"
#include "../../TaskManager/Base/task.h"
#include "../../TaskManager/Base/taskManager.h"
#include "../Base/scheduling.h"
#include "../../multiDeviceMgmt/deviceMgmt/deviceExploration.h"


//msg size starts in 16 MB and duplicates up to 128 MB
#define MIN_SIZE (1<<24) //16 MB
#define MAX_SIZE (1<<25) //32 MB
#define ONEGB (1<<30)
#define MAX_BW_REPS 10 //usually 4 repetitions are enough to achieve the 128 MB.
#define	LATENCY_REPS 10
#define ROOT 0

int msgSz;
int i,j;
int src,dst;
int err;

float L;
float* L_Mtx;

float BW;
float* BW_Mtx;
int numBWTrials;
float sumAvgs;
float Avg[MAX_BW_REPS];

double T1, T2,              /* start/end times per rep */
accumTime,                   /* sum of all reps times */
deltaT;                 /* time for one rep */


#endif /* COMMSBENCH_H_ */
