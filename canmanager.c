/*
 * canamanger.c
 *
 *  Created on: 28 gru 2016
 *      Author: Luke
 */
#include "canmanager.h"
#include "ch.h"
#include "hal.h"
#include "globals.h"
#include "serialmanager.h"
#include "inttypes.h"
#include "buffer.h"
#include "temperaturemanager.h"
#include "speedmanager.h"

#define CAN_SERIAL_DEBUG 0

// rpm, total_current, duty_cycle
#define CAN_PACKET_STATUS 9
#define CAN_PACKET_STATUS_SIZE 8
// total_current_filtered, supply_voltage, amp_hours_drawn, amp_hours_charged
#define CAN_PACKET_STATUS_2 10
#define CAN_PACKET_STATUS_2_SIZE 8
// temperature
#define CAN_PACKET_STATUS_3 11
#define CAN_PACKET_STATUS_3_SIZE 2
// lights
#define CAN_FRONT_LIGHTS_PACKET 16
#define CAN_FRONT_LIGHTS_PACKET_SIZE 2
#define CAN_REAR_LIGHTS_PACKET 15
#define CAN_REAR_LIGHTS_PACKET_SIZE 3


CANRxFrame rxMsg;

/*
 * 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 */
static const CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_SJW(1) | CAN_BTR_TS2(7) |
  CAN_BTR_TS1(6) | CAN_BTR_BRP(5)
};

void cm_init(void)
{
  canStart(&CAND1, &cancfg);
}

void cm_run(void)
{
  while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxMsg, TIME_IMMEDIATE) == MSG_OK) {
    palToggleLine(LINE_LED_GREEN);
    uint32_t eid = rxMsg.EID;
    uint8_t packetId = (eid & 0x0000FF00) >> 8;
    uint8_t deviceId = eid & 0x000000FF;
#if CAN_SERIAL_DEBUG
    sm_chprintf("deviceId= %d, packetId= %d\n\r", deviceId, packetId);
#endif
    if(packetId == CAN_PACKET_STATUS) {
      CANPacket1 packet = cm_unpackPacket1(rxMsg);
      sm_onNewVESCPacket(packet, deviceId);
    } else if(packetId == CAN_PACKET_STATUS_2) {
      CANPacket2 packet = cm_unpackPacket2(rxMsg);
      bm_onNewVESCPacket(packet, deviceId);
    } else if(packetId == CAN_PACKET_STATUS_3) {
      CANPacket3 packet = cm_unpackPacket3(rxMsg);
      tm_onNewTemperaturePacket(packet, deviceId);
    } else {
      // unknown error
    }
  }
}

void cm_sendLightsPacket(CANLightsPacket packet)
{
  CANTxFrame txMsg;
  // front lights packet
  txMsg.IDE = CAN_IDE_EXT;
  txMsg.EID = cm_getEID(CU_ID, CAN_FRONT_LIGHTS_PACKET);
#if CAN_SERIAL_DEBUG
  sm_chprintf("Sending packet, eid= %d\n\r", txMsg.EID);
#endif
  txMsg.RTR = CAN_RTR_DATA;
  txMsg.DLC = CAN_FRONT_LIGHTS_PACKET_SIZE;
  txMsg.data8[0] = packet.frontBrightness;
  txMsg.data8[1] = packet.frontBlinkingMode;
  cm_sendFrame(txMsg);

  // rear lights packet
  txMsg.IDE = CAN_IDE_EXT;
  txMsg.EID = cm_getEID(CU_ID, CAN_REAR_LIGHTS_PACKET);
  #if CAN_SERIAL_DEBUG
    sm_chprintf("Sending packet, eid= %d\n\r", txMsg.EID);
  #endif
  txMsg.RTR = CAN_RTR_DATA;
  txMsg.DLC = CAN_REAR_LIGHTS_PACKET_SIZE;
  txMsg.data8[0] = packet.rearBrightness;
  txMsg.data8[1] = packet.reactToBraking;
  txMsg.data8[2] = packet.rearBlinkingMode;
  cm_sendFrame(txMsg);

}

CANPacket1 cm_unpackPacket1(CANRxFrame frame)
{
  CANPacket1 packet;
  packet.dutyCycle = 0;
  packet.rpm = 0;
  packet.totalCurrent = 0;
#if CAN_SERIAL_DEBUG
  sm_chprintf("%d, CANPacket1\n\r", ST2MS(chVTGetSystemTime()));
#endif
  if(frame.DLC != CAN_PACKET_STATUS_SIZE) {
#if CAN_SERIAL_DEBUG
    sm_chprintf("INCORRECT DATA LEN: %d\n\r", frame.DLC);
#endif
    return packet;
  }
  int32_t index = 0;
  packet.rpm = bufferGetInt32(frame.data8, &index);
  packet.totalCurrent = bufferGetInt16(frame.data8, &index);
  packet.dutyCycle = bufferGetInt16(frame.data8, &index);
#if CAN_SERIAL_DEBUG
  sm_chprintf("RPM=%d\n\r", packet.rpm);
#endif
  return packet;
}

CANPacket2 cm_unpackPacket2(CANRxFrame frame)
{
  CANPacket2 packet;
  packet.ampHoursCharged = 0;
  packet.ampHoursDrawn = 0;
  packet.supplyVoltage = 0;
  packet.totCurrentFiltered = 0;
#if CAN_SERIAL_DEBUG
  sm_chprintf("%d, CANPacket2\n\r", ST2MS(chVTGetSystemTime()));
#endif
  if(frame.DLC != CAN_PACKET_STATUS_2_SIZE) {
#if CAN_SERIAL_DEBUG
    sm_chprintf("INCORRECT DATA LEN: %d\n\r", frame.DLC);
#endif
    return packet;
  }
  int32_t index = 0;
  packet.totCurrentFiltered = bufferGetInt16(frame.data8, &index);
  packet.supplyVoltage = bufferGetInt16(frame.data8, &index);
  packet.ampHoursDrawn = bufferGetUInt16(frame.data8, &index);
  packet.ampHoursCharged = bufferGetUInt16(frame.data8, &index);
#if CAN_SERIAL_DEBUG
  sm_chprintf("TotalCurrentFiltered=%d\n\r", packet.totCurrentFiltered);
  sm_chprintf("SupplyVoltage=%d\n\r", packet.supplyVoltage);
  sm_chprintf("AmpHoursDrawn=%d\n\r", packet.ampHoursDrawn);
#endif
  return packet;
}

CANPacket3 cm_unpackPacket3(CANRxFrame frame)
{
  CANPacket3 packet;
  packet.temperature = 0;
#if CAN_SERIAL_DEBUG
  sm_chprintf("%d, CANPacket3\n\r", ST2MS(chVTGetSystemTime()));
#endif
  if(frame.DLC != CAN_PACKET_STATUS_3_SIZE) {
#if CAN_SERIAL_DEBUG
    sm_chprintf("INCORRECT DATA LEN: %d\n\r", frame.DLC);
#endif
    return packet;
  }
  int32_t index = 0;
  packet.temperature = bufferGetInt16(frame.data8, &index);
#if CAN_SERIAL_DEBUG
  sm_chprintf("Temperature=%d\n\r", packet.temperature);
#endif
  return packet;
}

uint32_t cm_getEID(uint8_t deviceId, uint8_t messageId)
{
  uint32_t eid = (((uint32_t)messageId) << 8) | deviceId;
  return eid;
}

void cm_sendFrame(CANTxFrame frame)
{
  canTransmit(&CAND1, CAN_ANY_MAILBOX, &frame, MS2ST(100));
}
