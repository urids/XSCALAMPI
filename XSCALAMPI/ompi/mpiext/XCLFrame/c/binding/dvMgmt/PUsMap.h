/*
 * PUsMap.h
 *
 *  Created on: Sep 9, 2015
 *      Author: uriel
 */

#ifndef PUSMAP_H_
#define PUSMAP_H_

typedef struct PUInfo_st{
	int g_PUIdx;
	int l_PUIdx;
	int r_rank;
}PUInfo;


extern PUInfo* g_PUList; //defined in late.c
extern int g_numDevs; //defined in mpiext_XCL_Frame_late.c


#endif /* PUSMAP_H_ */
