/*
 * analogmanager.h
 *
 *  Created on: 29 gru 2016
 *      Author: Luke
 */

#ifndef ANALOGMANAGER_H_
#define ANALOGMANAGER_H_

#include <stdint.h>


void am_init(void);
uint16_t am_getReading(uint8_t channel);
float am_getChannelVoltage(uint8_t channel);


#endif /* ANALOGMANAGER_H_ */
