/*
 * buffer.c
 *
 *  Created on: 29 gru 2016
 *      Author: Luke
 */
#include "buffer.h"

int16_t bufferGetInt16(const uint8_t *buffer, int32_t *index)
{
  int16_t res = ((uint16_t) buffer[*index]) << 8 |
                ((uint16_t) buffer[*index + 1]);
  *index += 2;
  return res;
}

uint16_t bufferGetUInt16(const uint8_t *buffer, int32_t *index)
{
  uint16_t res =    ((uint16_t) buffer[*index]) << 8 |
                    ((uint16_t) buffer[*index + 1]);
  *index += 2;
  return res;
}

int32_t bufferGetInt32(const uint8_t *buffer, int32_t *index)
{
  int32_t res =   ((uint32_t) buffer[*index]) << 24 |
                  ((uint32_t) buffer[*index + 1]) << 16 |
                  ((uint32_t) buffer[*index + 2]) << 8 |
                  ((uint32_t) buffer[*index + 3]);
  *index += 4;
  return res;
}

void bufferAppendUInt8(uint8_t* buffer, uint8_t number, int32_t *index)
{
  buffer[(*index)++] = number;
}

void bufferAppendInt16(uint8_t* buffer, int16_t number, int32_t *index)
{
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

void bufferAppendUInt16(uint8_t* buffer, uint16_t number, int32_t *index)
{
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}
