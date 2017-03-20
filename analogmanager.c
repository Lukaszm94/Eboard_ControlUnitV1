/*
 * analogmanager.c
 *
 *  Created on: 29 gru 2016
 *      Author: Luke
 */
#include "analogmanager.h"
#include "hal.h"
#include "globals.h"
#include "serialmanager.h"

#define AM_SERIAL_DEBUG 0

void am_init(void)
{
  static const ADCConversionGroup adcConversionGroup = {
    TRUE,                            /*CIRCULAR*/
    AM_CHANNELS_COUNT,                        /*NUMB OF CH*/
    NULL,                             /*NO ADC CALLBACK*/
    NULL,                             /*NO ADC ERROR CALLBACK*/
    ADC_CFGR1_CONT | ADC_CFGR1_RES_12BIT,
    ADC_TR(0, 0),                                     /* TR */
    ADC_SMPR_SMP_239P5,                                /* SMPR */
    ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2 |
    ADC_CHSELR_CHSEL3 | ADC_CHSELR_CHSEL4 | ADC_CHSELR_CHSEL5 |
    ADC_CHSELR_CHSEL6                                     /* CHSELR */
  };
  adcStart(&ADCD1, NULL);
  adcStartConversion(&ADCD1, &adcConversionGroup, am_dataBuffer, AM_SAMPLES_COUNT);
}

uint16_t am_getReading(uint8_t channel)
{
  if(channel >= AM_CHANNELS_COUNT) {
#if AM_SERIAL_DEBUG
    sm_chprintf("am_getReading(): channel number too high: %d\n\r", channel);
#endif
    return 0;
  }
  uint32_t sum = 0;
  int i = 0;
  for(i = 0; i < AM_SAMPLES_COUNT; i++) {
    sum += am_dataBuffer[channel + i * AM_CHANNELS_COUNT];
  }
  sum += AM_SAMPLES_COUNT / 2;
  uint16_t result = sum / AM_SAMPLES_COUNT;
  return result;
}

float am_getChannelVoltage(uint8_t channel)
{
  if(channel >= AM_CHANNELS_COUNT) {
#if AM_SERIAL_DEBUG
    sm_chprintf("am_getChannelVoltage(): channel number too high: %d\n\r", channel);
#endif
    return 0.0;
  }
  float sum = 0;
  int i = 0;
  for(i = 0; i < AM_SAMPLES_COUNT; i++) {
    sum += am_dataBuffer[channel + i * AM_CHANNELS_COUNT];
  }
  float voltage = sum / (float)(AM_ADC_MAX_VALUE * AM_SAMPLES_COUNT) * AM_ADC_REF_VOLTAGE;
#if AM_SERIAL_DEBUG
  sm_chprintf("am_getChannelVoltage(): channel %d voltage*100: %d\n\r", channel, (int)(voltage*100));
#endif
  return voltage;
}

