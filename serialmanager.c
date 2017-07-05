/*
 * serialmanager.c
 *
 *  Created on: 31 gru 2016
 *      Author: Luke
 */
#include "serialmanager.h"
#include "globals.h"
#include "chprintf.h"
#include "hal.h"
#include "buffer.h"
#include "canmanager.h"

#define SM_SERIAL_DEBUG 0

#define CURRENT_PACKET_HEADER 'a'
#define TEMPERATURE_PACKET_HEADER 'b'
#define BATTERY_PACKET_HEADER 'c'
#define SPEED_PACKET_HEADER 'd'
#define ERROR_PACKET_HEADER 'e'

#define LIGHTS_PACKET_HEADER 'l'

#define PACKET_END_CHAR ';'

#define BUFFER_SIZE 16

void sm_init(void)
{
  sdStart(&SD1, NULL);
}

void sm_sendDataPackets(void)
{
  sm_sendCurrentPacket();
  sm_sendTemperaturePacket();
  sm_sendBatteryPacket();
  sm_sendSpeedPacket();
}

void sm_sendErrorPacket(ErrorPacket packet)
{
  //TODO
}

int sm_chprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    if(chMtxTryLock(&serialMutex) == TRUE) {
      chvprintf(chp, format, args);
      chMtxUnlock(&serialMutex);
    }
    va_end(args);
    return 0;
}

void sm_receiveData(void)
{
  uint8_t buffer[BUFFER_SIZE];
  int bytesRead = chnReadTimeout(&SD1, buffer, BUFFER_SIZE, TIME_IMMEDIATE);
  if(bytesRead != 7) {
    return;
  }
  if(buffer[0] != LIGHTS_PACKET_HEADER) {
#if SM_SERIAL_DEBUG
    sm_chprintf("Buffer header incorrect %c\n\r", buffer[0]);
#endif
    return;
  }
  if(buffer[6] != PACKET_END_CHAR) {
#if SM_SERIAL_DEBUG
    sm_chprintf("Buffer end char incorrect %c\n\r", buffer[3]);
#endif
    return;
  }
  CANLightsPacket packet;
  packet.frontBrightness = buffer[1];
  packet.frontBlinkingMode = buffer[2];
  packet.rearBrightness = buffer[3];
  packet.rearBlinkingMode = buffer[4];
  packet.reactToBraking = buffer[5];

#if SM_SERIAL_DEBUG
  sm_chprintf("Received correct lights packet, frontBrightness: %d,frontMode: %d, rearBrightness: %d, rearMode: %d, braking: %d\n\r", packet.frontBrightness, packet.frontBlinkingMode, packet.rearBrightness, packet.rearBlinkingMode, packet.reactToBraking);
#endif
  cm_sendLightsPacket(packet);
}

void sm_sendCurrentPacket(void)
{
  if(chMtxTryLock(&currentPacketMutex) != TRUE) {
    return;
  }
  uint8_t buffer[BUFFER_SIZE];
  int32_t index = 0;
  bufferAppendUInt8(buffer, CURRENT_PACKET_HEADER, &index);
  bufferAppendInt16(buffer, currentPacket.VESC1Current, &index);
  bufferAppendInt16(buffer, currentPacket.VESC2Current, &index);
  bufferAppendUInt8(buffer, PACKET_END_CHAR, &index);
  chMtxUnlock(&currentPacketMutex);
  sm_sendBuffer(buffer, index);
}

void sm_sendTemperaturePacket(void)
{
  if(chMtxTryLock(&temperaturePacketMutex) != TRUE) {
    return;
  }
  uint8_t buffer[BUFFER_SIZE];
  int32_t index = 0;
  bufferAppendUInt8(buffer, TEMPERATURE_PACKET_HEADER, &index);
  bufferAppendInt16(buffer, temperaturePacket.VESC1Temperature, &index);
  bufferAppendInt16(buffer, temperaturePacket.VESC2Temperature, &index);
  bufferAppendInt16(buffer, temperaturePacket.powerSwitchTemperature, &index);
  bufferAppendInt16(buffer, temperaturePacket.driversUnitCaseTemperature, &index);
  bufferAppendUInt8(buffer, PACKET_END_CHAR, &index);
  chMtxUnlock(&temperaturePacketMutex);
  sm_sendBuffer(buffer, index);
}

void sm_sendBatteryPacket(void)
{
  if(chMtxTryLock(&batteryPacketMutex) != TRUE) {
    return;
  }
  uint8_t buffer[BUFFER_SIZE * 2];
  int32_t index = 0;
  bufferAppendUInt8(buffer, BATTERY_PACKET_HEADER, &index);
  int i = 0;
  for(i = 0; i < BM_BATTERY_CELLS_COUNT; i++) {
    bufferAppendUInt16(buffer, batteryPacket.cellsVoltage[i], &index);
  }
  bufferAppendUInt16(buffer, batteryPacket.CUBatteryVoltage, &index);
  bufferAppendUInt16(buffer, batteryPacket.VESCBatteryVoltage, &index);
  bufferAppendUInt16(buffer, batteryPacket.ampHoursDrawn, &index);
  bufferAppendUInt16(buffer, batteryPacket.ampHoursCharged, &index);
  bufferAppendUInt8(buffer, batteryPacket.batteryCharge, &index);
  bufferAppendUInt8(buffer, PACKET_END_CHAR, &index);
  chMtxUnlock(&batteryPacketMutex);
  sm_sendBuffer(buffer, index);
}

void sm_sendSpeedPacket(void)
{
  if(chMtxTryLock(&speedPacketMutex) != TRUE) {
    return;
  }
  uint8_t buffer[BUFFER_SIZE];
  int32_t index = 0;
  bufferAppendUInt8(buffer, SPEED_PACKET_HEADER, &index);
  bufferAppendUInt16(buffer, speedPacket.speed, &index);
  bufferAppendUInt8(buffer, PACKET_END_CHAR, &index);
  chMtxUnlock(&speedPacketMutex);
  sm_sendBuffer(buffer, index);
}

/*
 *
 * \return -1 if not ok, 0 if ok
 */

int sm_sendBuffer(uint8_t* data, int length)
{
  if(chMtxTryLock(&serialMutex) != TRUE) {
    return -1;
  }
  int bytesSend = chnWriteTimeout(&SD1, data, length, 1000);
  chMtxUnlock(&serialMutex);
  if(bytesSend != length) {
#if SM_SERIAL_DEBUG
    sm_chprintf("Sent %d bytes instead of %d\n\r", bytesSend, length);
#endif
    return -1;
  }
  return 0;
}
