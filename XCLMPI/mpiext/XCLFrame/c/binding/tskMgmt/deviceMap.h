/*
 * deviceMap.h
 *
 *  Created on: Apr 29, 2015
 *      Author: uriel
 */

#ifndef DEVICEMAP_H_
#define DEVICEMAP_H_

typedef struct device_Task_Info_st{
 Device* mappedDevice;
 //int l_deviceIdx;
 int min_tskIdx;
 int max_tskIdx;
}device_Task_Info;

extern device_Task_Info* taskDevMap;

#endif /* DEVICEMAP_H_ */
