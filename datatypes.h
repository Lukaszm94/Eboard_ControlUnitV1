/*
 * datatypes.h
 *
 *  Created on: 27 gru 2016
 *      Author: Luke
 */

#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <stdint.h>
#include "config.h"

typedef struct CurrentPacket {
  int16_t VESC1Current; // *10
  int16_t VESC2Current; // *10
} CurrentPacket;

typedef struct TemperaturePacket {
  int16_t VESC1Temperature; // *100
  int16_t VESC2Temperature; // *100
  int16_t powerSwitchTemperature; // *100
  int16_t driversUnitCaseTemperature; // *100
} TemperaturePacket;

typedef struct BatteryPacket {
  uint16_t cellsVoltage[BM_BATTERY_CELLS_COUNT]; // *100
  uint16_t CUBatteryVoltage; // *100
  uint16_t VESCBatteryVoltage; // *100
  uint16_t ampHoursDrawn; // *1000
  uint16_t ampHoursCharged; // *1000
  uint8_t batteryCharge; // *1, in %
} BatteryPacket;

typedef struct SpeedPacket {
  uint16_t speed; // *100, in m/s
} SpeedPacket;

typedef struct ErrorPacket {
  uint16_t module;
  uint16_t code;
} ErrorPacket;

typedef struct CANPacket1 {
  int32_t rpm;
  int16_t totalCurrent; // *10
  int16_t dutyCycle; // *1000
} CANPacket1;

typedef struct CANPacket2 {
  int16_t totCurrentFiltered; // *10
  int16_t supplyVoltage; // *10
  uint16_t ampHoursDrawn; // *1000
  uint16_t ampHoursCharged; // *1000
} CANPacket2;

typedef struct CANPacket3 {
  int16_t temperature; // *100
} CANPacket3;

typedef struct CANLightsPacket {
  uint8_t brightness; // in percent, *1
  uint8_t reactToBraking; // 0-no, 1-yes
  uint8_t blinkingMode;
} CANLightsPacket;

typedef struct VESCBatteryData {
  uint16_t ampHoursDrawn;
  uint16_t ampHoursCharged;
  uint16_t supplyVoltage;
  int16_t totCurrentFiltered;
} VESCBatteryData;

#endif /* DATATYPES_H_ */
