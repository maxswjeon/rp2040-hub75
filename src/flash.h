/*
 * Flash Layout
 *
 *         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
 *   0  |     Checksum (SKKU)   |      SSID Offset      |       SSID Length     |      PSK  Offset     |
 *   1  |       PSK Length      |
 *   2
 *   3
 *   4
 */
#ifndef _SKKU_BUS_FLASH_H_
#define _SKKU_BUS_FLASH_H_

#include "pico/stdlib.h"

#define FLASH_CHECKSUM 0x534B4B55

uint32_t flash_read_uint32(const uint8_t *data, uint8_t offset)
{
    return data[offset] << 24 | data[offset + 1] << 16 | data[offset + 2] << 8 | data[offset + 3];
}

#endif // _SKKU_BUS_FLASH_H_