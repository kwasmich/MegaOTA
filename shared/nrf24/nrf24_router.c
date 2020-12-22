//
//  nrf24_router.c
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "nrf24_router.h"

#include "nrf24.h"
#include "nrf24_packets.h"
#include "nrf24_node.h"

#include <stdio.h>

//static const uint8_t

void nrf24_router_init() {
    nrf24_init();
    nrf24_set_ard(2);                             // TODO: RND [tier+1…15]
//    nrf24_set_pipe_address(0, 0x33);
//    nrf24_set_pipe_address(1, 0x55);
//    nrf24_set_pipe_address(2, 0x66);
//    nrf24_set_pipe_address(3, 0x99);
//    nrf24_set_pipe_address(4, 0xAA);
//    nrf24_set_pipe_address(5, 0x00);            // 0xCC
}



bool nrf24_router_rx(uint8_t * const out_len, uint8_t out_payload[static const 32]) {

//    uint8_t pipe;
//    RawPacket_t *pkg = (RawPacket_t *)out_payload;
//
//    if (nrf24_rx(&pipe, out_len, out_payload)) {
//        puts("received");
//        printf("pipe: %d\nlen : %d\ndata: ", pipe, *out_len);
//
//        for (uint8_t i = 0; i < *out_len; i++) {
//            putchar(out_payload[i]);
//        }
//
//        puts("");
//
//        if (pkg->dst < ADDR_DN_BROADCAST) {
//            nrf24_node_route(*out_len, pkg);
//            return false;
//        }
//
//        switch (pkg->dst) {
//            case ADDR_UP_ROOT:
//                return true;
//
//            default:
//                puts("dunno what to do");
//                break;
//        }
//    }

    return false;
}
