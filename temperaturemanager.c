/*
 * temperaturemanager.c
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */
#include "temperaturemanager.h"
#include "globals.h"
#include "config.h"
#include "analogmanager.h"
#include "serialmanager.h"
#include "math.h"

#define TM_SERIAL_DEBUG 0

//#define NTC_RES(adc_val)    ((4095.0 * 10000.0) / adc_val - 10000.0)
// calculate NTC resistance
#define NTC_RES(adc_val)    ((10000.0 * adc_val) / (4095.0 - adc_val))
#define NTC_TEMP(adc_value)   (1.0 / ((logf(NTC_RES(adc_value) / 10000.0) / 3434.0) + (1.0 / 298.15)) - 273.15)

void tm_updateMosfetTemperature(void)
{
  uint16_t adcReading = am_getReading(TM_MOSFET_TEMP_CHANNEL);
  float temperature = NTC_TEMP(adcReading);
  int16_t temperatureI16 = (int16_t)(temperature * 100);
#if TM_SERIAL_DEBUG
  sm_chprintf("tm_updateMosfetTemperature, T=%d\n\r", temperatureI16);
#endif
  if(chMtxTryLock(&temperaturePacketMutex) == TRUE) {
    temperaturePacket.powerSwitchTemperature = temperatureI16;
    chMtxUnlock(&temperaturePacketMutex);
  }
}

void tm_onNewTemperaturePacket(CANPacket3 data, uint8_t deviceId)
{
  int error = 0;
  if(chMtxTryLock(&temperaturePacketMutex) == TRUE) {
      if(deviceId == VESC_1_ID) {
        temperaturePacket.VESC1Temperature = data.temperature;
      } else if(deviceId == VESC_2_ID) {
        temperaturePacket.VESC2Temperature = data.temperature;
      } else if(deviceId == DUC_ID) {
        temperaturePacket.driversUnitCaseTemperature = data.temperature;
      } else {
        error = 1;
      }
      chMtxUnlock(&temperaturePacketMutex);
    }
#if TM_SERIAL_DEBUG
  if(error)
    sm_chprintf("tm_onNewTemperaturePacket: unknown device Id: %d\n\r", deviceId);
#endif
}

