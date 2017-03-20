/*
 * batterymanager.h
 *
 *  Created on: 27 gru 2016
 *      Author: Luke
 */

#ifndef BATTERYMANAGER_H_
#define BATTERYMANAGER_H_

#include "datatypes.h"
#include "config.h"

void bm_updateCellVoltages(void);
void bm_onNewVESCPacket(CANPacket2 data, uint8_t vescId);
void bm_onSetBatteryCharge(uint8_t newCharge);
int bm_isBatteryOk(void);
// private
VESCBatteryData bm_loadVESCData(CANPacket2 data);
void bm_updateVESCDataInPackets(VESCBatteryData vesc1Data, VESCBatteryData vesc2Data);
int bm_isCellVoltageOk(float v);
float bm_calculateInputVoltage(float adcVoltage, float R_lower, float R_upper);

#endif /* BATTERYMANAGER_H_ */
