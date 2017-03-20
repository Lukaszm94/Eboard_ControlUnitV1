/*
 * buffer.h
 *
 *  Created on: 29 gru 2016
 *      Author: Luke
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>

int16_t bufferGetInt16(const uint8_t *buffer, int32_t *index);
uint16_t bufferGetUInt16(const uint8_t *buffer, int32_t *index);
int32_t bufferGetInt32(const uint8_t *buffer, int32_t *index);

void bufferAppendUInt8(uint8_t* buffer, uint8_t number, int32_t *index);
void bufferAppendInt16(uint8_t* buffer, int16_t number, int32_t *index);
void bufferAppendUInt16(uint8_t* buffer, uint16_t number, int32_t *index);

#endif /* BUFFER_H_ */
