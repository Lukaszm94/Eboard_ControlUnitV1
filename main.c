/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "datatypes.h"
#include "halconf.h"
#include "chprintf.h"
#include "batterymanager.h"
#include "powerswitchmanager.h"
#include "canmanager.h"
#include "analogmanager.h"
#include "temperaturemanager.h"
#include "serialmanager.h"

BaseSequentialStream *chp = (BaseSequentialStream *)&SD1;
//BaseChannel *bch = (BaseChannel*) &SD1;

BatteryPacket batteryPacket;
mutex_t batteryPacketMutex;

TemperaturePacket temperaturePacket;
mutex_t temperaturePacketMutex;

CurrentPacket currentPacket;
mutex_t currentPacketMutex;

SpeedPacket speedPacket;
mutex_t speedPacketMutex;

uint16_t am_dataBuffer[AM_CHANNELS_COUNT * AM_SAMPLES_COUNT];
mutex_t am_dataMutex;

mutex_t serialMutex;
event_listener_t can_el;


/*
 * CAN receiver thread, waits for rxfull event, then canmanager processes new packets
 */
static THD_WORKING_AREA(waThread1, 512);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("can_receiver");
  chEvtRegister(&CAND1.rxfull_event, &can_el, 0);
  while(true) {
    while(!chThdShouldTerminateX()) {
      if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
        continue;
      cm_run();
    }
    chEvtUnregister(&CAND1.rxfull_event, &can_el);
  }
}

/*
 * Serial communication thread
 */
static THD_WORKING_AREA(waThread2, 512);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("serial_comm");
  systime_t lastPacketSentTimestamp = chVTGetSystemTime();
  while(true) {
    // receive data
    sm_receiveData();
    // send data
    systime_t dt = chVTGetSystemTime() - lastPacketSentTimestamp;
    if(ST2MS(dt) > SM_PACKET_UPDATE_PERIOD) {
      sm_sendDataPackets();
      lastPacketSentTimestamp = chVTGetSystemTime();
    }
    // sleep
    chThdSleepMilliseconds(50);
  }
}

/*
 * Power switch thread
 */
static THD_WORKING_AREA(waThread3, 256);
static THD_FUNCTION(Thread3, arg) {

  (void)arg;
  chRegSetThreadName("power_switch");

  while(true) {
    /*if(!bm_isBatteryOk()) {
      chThdSleepMilliseconds(2000);
      if(!bm_isBatteryOk()) {
        ps_disable();
      }
      chThdSleepMilliseconds(2000);
    } else*/ if(!ps_isEnabled()) {
      chThdSleepMilliseconds(5000);
      ps_enable();
    }
    chThdSleepMilliseconds(2000);
  }
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */

  halInit();
  chSysInit();

  chMtxObjectInit(&batteryPacketMutex);
  chMtxObjectInit(&currentPacketMutex);
  chMtxObjectInit(&temperaturePacketMutex);
  chMtxObjectInit(&speedPacketMutex);
  chMtxObjectInit(&serialMutex);
  chMtxObjectInit(&am_dataMutex);

  sm_init();
  ps_init();
  cm_init();
  am_init();




  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO, Thread3, NULL);

  palSetLineMode(LINE_LED_GREEN, PAL_MODE_OUTPUT_PUSHPULL);
  palClearLine(LINE_LED_GREEN);

  while (true) {
    chThdSleepMilliseconds(50);
    bm_updateCellVoltages();
    tm_updateMosfetTemperature();
    palToggleLine(LINE_LED_GREEN);
  }
}
