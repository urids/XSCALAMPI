/*
 * ISH.h
 *
 *  Created on: Jul 23, 2016
 *      Author: uriel
 */

#ifndef ISH_H_
#define ISH_H_
#include <stack> //for topological sorting.
#include <list> //for topological sorting.
#include <vector> //for topological sorting.
#include <algorithm> // for for_each
#include <map>
#include <iostream>
#include <assert.h>
#include <string.h>
#include "IScheduler.hpp"
#include "Utilities/topoSort.h"


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

class ISH: public Scheduler {
public:
	ISH();
	virtual ~ISH();
	virtual int matchMake(int NumTsk, int NumDvs,float * W, int * Adj,const int* SR,const int* AS, int* schedule);
};

#endif /* ISH_H_ */
