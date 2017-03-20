/*
 * powerswitchmanager.c
 *
 *  Created on: 28 gru 2016
 *      Author: Luke
 */
#include "powerswitchmanager.h"
#include "hal.h"
#include "serialmanager.h"

#define PS_SERIAL_DEBUG 0

void ps_init(void)
{
  palSetLineMode(LINE_POWER_SWITCH, PAL_MODE_OUTPUT_PUSHPULL);
  ps_disable();
}

void ps_enable(void)
{
#if PS_SERIAL_DEBUG
  sm_chprintf("ps_enable\n\r");
#endif
  palSetLine(LINE_POWER_SWITCH);
}

void ps_disable(void)
{
#if PS_SERIAL_DEBUG
  sm_chprintf("ps_disable\n\r");
#endif
  palClearLine(LINE_POWER_SWITCH);
}

/*
 * \brief checks if Power Switch is enabled (turned on)
 * \return 0 if PS is off, 1 if PS is on
 */

int ps_isEnabled(void)
{
  if(palReadLine(LINE_POWER_SWITCH) == PAL_HIGH) {
    return 1;
  }
  return 0;
}

