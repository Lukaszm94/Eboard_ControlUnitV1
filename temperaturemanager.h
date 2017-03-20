/*
 * temperaturemanager.h
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */

#ifndef TEMPERATUREMANAGER_H_
#define TEMPERATUREMANAGER_H_

#include "datatypes.h"

void tm_updateMosfetTemperature(void);
void tm_onNewTemperaturePacket(CANPacket3 data, uint8_t deviceId);

#endif /* TEMPERATUREMANAGER_H_ */
