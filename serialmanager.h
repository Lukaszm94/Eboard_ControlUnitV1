/*
 * serialmanager.h
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */

#ifndef SERIALMANAGER_H_
#define SERIALMANAGER_H_
#include "datatypes.h"

void sm_init(void);
void sm_sendDataPackets(void);
void sm_sendErrorPacket(ErrorPacket packet);
int sm_chprintf(const char *format, ...);
void sm_receiveData(void);

//private
void sm_sendCurrentPacket(void);
void sm_sendTemperaturePacket(void);
void sm_sendBatteryPacket(void);
void sm_sendSpeedPacket(void);
int sm_sendBuffer(uint8_t* data, int length);

#endif /* SERIALMANAGER_H_ */
