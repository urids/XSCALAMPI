/*
 * common.h
 *
 *  Created on: Oct 31, 2016
 *      Author: uriel
 */

#ifndef COMMON_H_
#define COMMON_H_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#include "../Base/schedWrapper.h"

#include <time.h> //for rand
#include "vector"
#include <algorithm> //for_each
#include <math.h>
#include "dlfcn.h"
#include <iostream>
#include <random>
#include "string.h" //for memcpy

#include <cstdlib>
#include <cmath>
#include <limits>

using namespace std;

//RANDOM NUMBERS GENERATION
///////////////////////
int gammaNumber(float alpha);
double gaussianNumber(double mu, double sigma);
int U(int min, int max);
double U(double min, double max);


//RANDOM DAG GENERATION
///////////////////////

#define MIN_PER_LEVEL 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_LEVEL 5
#define MIN_LEVELS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_LEVELS 5
#define PERCENT 30     /* Chance of having an Edge.  */
#define MIN_EDGE_W 1    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_EDGE_W 10
#define REQ 128 //(MB) mean memory requirement  per task

int isSource(int i, int* Adj, int N);
int isSink(int i, int* Adj, int N);

class randApplication{
public:
	int numTasks;
	int *EadjMtx;
	int *r;
	randApplication();

private:

	void newRandDAG();
};

//RANDOM ENVIRONM GENERATION
///////////////////////

#define MIN_NODES 3
#define MAX_NODES 6
#define MIN_CPUS 1
#define MAX_CPUS 2
#define MIN_GPUS 1
#define MAX_GPUS 4
#define MIN_RANKS 1
#define MAX_RANKS 2
#define GPU_MIN_BW 5
#define GPU_MAX_BW 10

#define MIN_CPU_MEM 2
#define MAX_CPU_MEM 5 //(1<<5 = 32 GB)
#define MIN_GPU_MEM 1
#define MAX_GPU_MEM 3

//benchmark execution times.
#define MIN_GPU_BMARK 0.1
#define MAX_GPU_BMARK 0.8
#define MIN_CPU_BMARK 2
#define MAX_CPU_BMARK 6

enum devType{CPU=0, GPU=1};
typedef struct PU_s{
	int rank;
	int type;
	int ID;
}PU_t;

class execEnvironment{
public:
	execEnvironment();
	int numRanks;
	int numDevices;
	vector<PU_t> PUs;
	int* s;
	float* LMtx;
	float* BWMtx;
	float* AccMtx;
	char * U_TEST_Path="./mpiext/XCLFrame/Scheduling/StaticSched/U_TEST";

private:
	double X=1.0; // BW base (GB/s)
	double Y=.001; // L base (sec)
	double alpha,beta,gamma,theta; //memory hierarchies access factors

	void rule4(char flag, int i, int j);
	void build_Nodes();
	void build_Connections();
	void run_Benchmarks();
};

int* newRandDAG(int* numTasks);
PU_t* newRsrcList(int* numNodes);



#endif /* COMMON_H_ */
