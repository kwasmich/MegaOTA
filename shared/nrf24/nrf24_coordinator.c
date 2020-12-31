//
//  nrf24_coordinator.c
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "nrf24_coordinator.h"

#include "nrf24.h"
#include "nrf24_packets.h"
#include "nrf24_node.h"

#include <stdio.h>


//typedef struct {
//    uint8_t pipe0 : 1;
//    uint8_t pipe1 : 1;
//    uint8_t pipe2 : 1;
//    uint8_t pipe3 : 1;
//    uint8_t pipe4 : 1;
//    uint8_t pipe5 : 1;
//} state_s;
//
//
//
//static state_s s_state;



void nrf24_coordinator_init() {
    nrf24_init();
    nrf24_disable_pipe(0);
    nrf24_set_pipe_address(0, 0x33);
    nrf24_set_pipe_address(1, 0x55);
    nrf24_set_pipe_address(2, 0x66);
    nrf24_set_pipe_address(3, 0x99);
    nrf24_set_pipe_address(4, 0xAA);
    nrf24_set_pipe_address(5, 0x00);            // 0xCC
    nrf24_enable_pipe(5);
    nrf24_set_ard(1);                                                           // this is the lowest delay that allows for payloads up to 32 byte
}



bool nrf24_coordinator_rx(uint8_t * const out_len, uint8_t out_payload[static const 32]) {
    uint8_t pipe;
    RawPacket_t *pkg = (RawPacket_t *)out_payload;

    if (nrf24_rx(&pipe, out_len, out_payload)) {
//        puts("received");
//        printf("pipe: %d\nlen : %d\ndata: ", pipe, *out_len);
//
//        for (uint8_t i = 0; i < *out_len; i++) {
//            printf("%02x ", out_payload[i]);
//        }
//
//        puts("");

        if (pkg->dst < ADDR_DN_BROADCAST) {
            nrf24_node_route(*out_len, pkg);
            return false;
        }

        switch (pkg->dst) {
            case ADDR_UP_PING:
                return false;

            case ADDR_UP_ROOT:
                return true;


            default:
                puts("received");
                printf("pipe: %d\nlen : %d\ndata: ", pipe, *out_len);

                for (uint8_t i = 0; i < *out_len; i++) {
                    printf("%02x ", out_payload[i]);
                }

                puts("");
                break;
        }
    }

    return false;
}
