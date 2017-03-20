/*
 * speedmanager.h
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */

#ifndef SPEEDMANAGER_H_
#define SPEEDMANAGER_H_
#include "globals.h"

void sm_onNewVESCPacket(CANPacket1 data, uint8_t deviceId);
// private
float sm_calculateSpeed(int32_t erpm); // in m/s
void sm_updatePacketData(int32_t erpm1, int32_t erpm2);

#endif /* SPEEDMANAGER_H_ */
