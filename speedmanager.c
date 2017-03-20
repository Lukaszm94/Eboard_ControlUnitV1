/*
 * speedmanager.c
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */
#include "speedmanager.h"
#include "serialmanager.h"

#define PI 3.1415
#define I 4
#define TYRE_DIAMETER 0.18
#define S (PI * TYRE_DIAMETER)

#define SM_SERIAL_DEBUG 0

void sm_onNewVESCPacket(CANPacket1 data, uint8_t deviceId)
{
  static int32_t vesc1ERPM, vesc2ERPM;
  if(deviceId == VESC_1_ID) {
    vesc1ERPM = data.rpm;
  } else if(deviceId == VESC_2_ID) {
    vesc2ERPM = data.rpm;
  } else {
    //TODO error
    return;
  }
#if SM_SERIAL_DEBUG
  sm_chprintf("ERPM: vesc1: %d, vesc2: %d\n\r", vesc1ERPM, vesc2ERPM);
#endif
  sm_updatePacketData(vesc1ERPM, vesc2ERPM);
}

float sm_calculateSpeed(int32_t erpm)
{
  if(erpm == 0) {
    return 0;
  }
  float T = (7 * 60 * I) / (float)erpm;
  float v = S / T;
  return v;
}

void sm_updatePacketData(int32_t erpm1, int32_t erpm2)
{
  int32_t avgERPM = (erpm1 + erpm2) / 2;
  float speed = sm_calculateSpeed(avgERPM);
  if(speed < 0.0) {
    speed = -speed;
  }
  uint16_t speedU16 = speed * 100;
#if SM_SERIAL_DEBUG
  sm_chprintf("Speed= %d\n\r", speedU16);
#endif
  if(chMtxTryLock(&speedPacketMutex)) {
    speedPacket.speed = speedU16;
    chMtxUnlock(&speedPacketMutex);
  }
}

