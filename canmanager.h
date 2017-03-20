/*
 * canmanager.h
 *
 *  Created on: 28 gru 2016
 *      Author: Luke
 */

#ifndef CANMANAGER_H_
#define CANMANAGER_H_
#include "hal.h"
#include "datatypes.h"

void cm_init(void);
void cm_run(void);
void cm_sendLightsPacket(CANLightsPacket packet);

// private
CANPacket1 cm_unpackPacket1(CANRxFrame frame);
CANPacket2 cm_unpackPacket2(CANRxFrame frame);
CANPacket3 cm_unpackPacket3(CANRxFrame frame);

uint32_t cm_getEID(uint8_t deviceId, uint8_t messageId);
void cm_sendFrame(CANTxFrame frame);

#endif /* CANMANAGER_H_ */
