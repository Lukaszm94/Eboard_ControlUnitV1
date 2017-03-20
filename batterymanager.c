/*
 * batterymanager.c
 *
 *  Created on: 27 gru 2016
 *      Author: Luke
 */
#include "batterymanager.h"
#include "globals.h"
#include "canmanager.h"
#include "serialmanager.h"
#include "analogmanager.h"

#define BM_SERIAL_DEBUG 0

float R_upper = 100000;
float R_lower[] = {300000, 56000, 33000, 22000, 18000, 13000};
//float C[] = {1.069, 0.92853, 1.02562, 0.95811, 1.055525, 0.9151};
float C[] = {1, 1, 0.9973, 0.996, 0.99424, 0.989};

void bm_updateCellVoltages(void)
{
  float cellInputVoltages[BM_BATTERY_CELLS_COUNT];
  uint16_t cellVoltages[BM_BATTERY_CELLS_COUNT];
  int i = 0;
  for(i = 0; i < BM_BATTERY_CELLS_COUNT; i++) {
    cellInputVoltages[BM_BATTERY_CELLS_COUNT - 1 - i] = am_getChannelVoltage(i);
  }
  for(i = 0; i < BM_BATTERY_CELLS_COUNT; i++) {
    float pinVoltage = cellInputVoltages[i];
    float cellVccVoltage = bm_calculateInputVoltage(pinVoltage * C[i], R_lower[i], R_upper);
    float cellGndVoltage = 0.0;
    if(i > 0) {
      cellGndVoltage = bm_calculateInputVoltage(cellInputVoltages[i - 1] * C[i - 1], R_lower[i - 1], R_upper);
    }
    float cellVoltage = cellVccVoltage - cellGndVoltage;
    if(!bm_isCellVoltageOk(cellVoltage)) {
#if BM_SERIAL_DEBUG
      sm_chprintf("Cell %d voltage not ok: %d. VCC=%d, GND=%d\n\r", i, (int)(cellVoltage*100), (int)(cellVccVoltage*100), (int)(cellGndVoltage*100));
#endif
      cellVoltage = 0.0;
    } else {
#if BM_SERIAL_DEBUG
      sm_chprintf("Cell %d voltage ok: %d. VCC=%d, GND=%d\n\r", i, (int)(cellVoltage*100), (int)(cellVccVoltage*100), (int)(cellGndVoltage*100));
#endif
    }
    cellVoltages[i] = (uint16_t)(cellVoltage * 100);
  }
  float batteryVoltage = bm_calculateInputVoltage(am_getChannelVoltage(BM_BATTERY_CELLS_COUNT - 1), R_lower[BM_BATTERY_CELLS_COUNT - 1], R_upper) * C[BM_BATTERY_CELLS_COUNT - 1];
  uint16_t batteryVoltageU16 = (uint16_t)(batteryVoltage * 100);
  if(batteryVoltage < BM_MIN_BATTERY_VOLTAGE) {
#if BM_SERIAL_DEBUG
    sm_chprintf("Battery voltage too low: %d\n\r", batteryVoltageU16);
#endif
  }

  chMtxTryLock(&batteryPacketMutex);
  for(i = 0; i < BM_BATTERY_CELLS_COUNT; i++) {
    batteryPacket.cellsVoltage[i] = cellVoltages[i];
  }
  batteryPacket.CUBatteryVoltage = batteryVoltageU16;
  chMtxUnlock(&batteryPacketMutex);
}

void bm_onNewVESCPacket(CANPacket2 data, uint8_t vescId)
{
  static VESCBatteryData vesc1Data, vesc2Data;
  (void) data;
  if(vescId == VESC_1_ID) {
    vesc1Data = bm_loadVESCData(data);
  } else if(vescId == VESC_2_ID) {
    vesc2Data = bm_loadVESCData(data);
  }
  bm_updateVESCDataInPackets(vesc1Data, vesc2Data);
}

void bm_onSetBatteryCharge(uint8_t newCharge)
{
  (void) newCharge;
}

/*
 * \brief check if battery is ok (cells voltage above threshold)
 * \return 1 if ok, 0 if not ok
 */
int bm_isBatteryOk(void)
{
  if(chMtxTryLock(&batteryPacketMutex) == FALSE) {
    return 1;
  }
  int ret = 1;
  int i = 0;
  for(i = 0; i < BM_BATTERY_CELLS_COUNT; i++) {
    uint16_t cellVoltage = batteryPacket.cellsVoltage[i];
    float v = cellVoltage / 100.0;
    if((v < BM_MIN_BATTERY_CELL_VOLTAGE) || (v > BM_MAX_BATTERY_CELL_VOLTAGE)) {
      ret = 0;
      break;
    }
  }
  chMtxUnlock(&batteryPacketMutex);
#if BM_SERIAL_DEBUG
  sm_chprintf("Battery ok: %d\n\r", ret);
#endif
  return ret;
}

VESCBatteryData bm_loadVESCData(CANPacket2 data)
{
  VESCBatteryData vescData;
  vescData.ampHoursCharged = data.ampHoursCharged;
  vescData.ampHoursDrawn = data.ampHoursDrawn;
  vescData.supplyVoltage = data.supplyVoltage;
  vescData.totCurrentFiltered = data.totCurrentFiltered;
  return vescData;
}

void bm_updateVESCDataInPackets(VESCBatteryData vesc1Data, VESCBatteryData vesc2Data)
{
  if(chMtxTryLock(&batteryPacketMutex) == TRUE) {
      batteryPacket.ampHoursCharged = vesc1Data.ampHoursCharged + vesc2Data.ampHoursCharged;
      batteryPacket.ampHoursDrawn = vesc1Data.ampHoursDrawn + vesc2Data.ampHoursDrawn;
      batteryPacket.VESCBatteryVoltage = (vesc1Data.supplyVoltage + vesc2Data.supplyVoltage) * 10 / 2;
      chMtxUnlock(&batteryPacketMutex);
  }
  if(chMtxTryLock(&currentPacketMutex) == TRUE) {
    currentPacket.VESC1Current = vesc1Data.totCurrentFiltered;
    currentPacket.VESC2Current = vesc2Data.totCurrentFiltered;
    chMtxUnlock(&currentPacketMutex);
  }
}

/*
 * \brief check if cell voltage is ok (cells voltage above or below threshold)
 * \return 1 if ok, 0 if not ok
 */
int bm_isCellVoltageOk(float v)
{
  if((v > BM_MIN_BATTERY_CELL_VOLTAGE) && (v < BM_MAX_BATTERY_CELL_VOLTAGE)) {
    return 1;
  }
  return 0;
}

float bm_calculateInputVoltage(float adcVoltage, float R_lower, float R_upper)
{
  return adcVoltage * (R_upper + R_lower) / R_lower;
}

