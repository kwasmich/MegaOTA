//
//  crc.c
//  updater
//
//  Created by Michael Kwasnicki on 25.11.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "crc.h"

#include <iso646.h>


// this is smaller than the version from <util/crc16.h>
// CRC-16/MODBUS
uint16_t crc16_update(uint16_t crc, uint8_t const a) {
    crc ^= a;

    for (uint8_t i = 0; i < 8; i++) {
        if (crc bitand 1) {
            crc = (crc >> 1) xor 0xA001;
        } else {
            crc = (crc >> 1);
        }
    }

    return crc;
}
