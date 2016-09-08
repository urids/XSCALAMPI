/*
 * utilities.h
 *
 *  Created on: Aug 17, 2016
 *      Author: uriel
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stack> //for topological sorting.
#include <list> //for topological sorting.
#include <iostream>
#include <string.h>
#define ONEGB (1<<30)
using namespace std;



#define transpose(A,A_T,LDA) \
		do {\
			for(int i=0;i<LDA;++i){	\
				for(int j=0;j<LDA;++j){ \
					A_T[LDA*j+i]=A[LDA*i+j]; \
				}\
			} \
		} while(0)

#define buildAdjList(A,AdjList,LDA) \
		do {\
			for(i=0;i<LDA;i++){ \
				for(j=0;j<LDA;j++){ \
					if(A[LDA*i+j]){ \
						AdjList[i].push_back(make_pair(j,A[LDA*i+j])); \
					} \
				} \
			} \
		} while(0)



int topoSort(int V, list<pair<int,int> > * adj, stack<int> &Stack);
void buildExtendedAdj(int* EAdj, int* Adj , int N);
void buildExtendedWij(float* EW, float* W , int numTasks, int numDvs);
float get_LAvg(double * L_Mtx,int g_NumPUs);
float get_BWAvg(double* BW_Mtx,int g_NumPUs);

#endif /* UTILITIES_H_ */
