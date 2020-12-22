//
//  nrf24_packets.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_packets_h
#define nrf24_packets_h


#include <stdint.h>

typedef enum {
    ADDR_DN_BROADCAST       = 0xD8,
    ADDR_DN_RESPONSE_JOIN   = 0xD9,
    ADDR_DN_REQUEST_LEAVE   = 0xDA,
    ADDR_DN_RESPONSE_LEAVE  = 0xDB,
    ADDR_RESERVED_START     = 0xDC,
    ADDR_RESERVED_END       = 0xF8,
    ADDR_UP_RESPONSE_LEAVE  = 0xF9,
    ADDR_UP_REQUEST_LEAVE   = 0xFA,
    ADDR_UP_REQUEST_JOIN    = 0xFB,
    ADDR_UP_ACQUIRE_ACK     = 0xFC,
    ADDR_UP_PREPARE_ACK     = 0xFD,
    ADDR_UP_PING            = 0xFE,
    ADDR_UP_ROOT            = 0xFF,
} Address_t;



typedef struct {
    uint8_t dst;
    uint8_t src;
    uint8_t payload[];
} RawPacket_t;



#endif /* nrf24_packets_h */
