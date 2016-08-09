/*
 * topoSort.h
 *
 *  Created on: Jul 25, 2016
 *      Author: uriel
 */

#ifndef TOPOSORT_H_
#define TOPOSORT_H_
#include <stack> //for topological sorting.
#include <list> //for topological sorting.
#include <iostream>
using namespace std;

int topoSort(int V, list<pair<int,int> > * adj, stack<int> &Stack);

#endif /* TOPOSORT_H_ */
