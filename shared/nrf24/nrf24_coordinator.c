//
//  nrf24_coordinator.c
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "nrf24_coordinator.h"

#include "nrf24.h"


typedef struct {
    uint8_t pipe0 : 1;
    uint8_t pipe1 : 1;
    uint8_t pipe2 : 1;
    uint8_t pipe3 : 1;
    uint8_t pipe4 : 1;
    uint8_t pipe5 : 1;
} state_s;



static state_s s_state;



void nrf24_coordinator_init() {
    nrf24_init();
//    uint8_t addr_pipe0[2] = { 0x33, 0x00 };
//    uint8_t addr_pipe1[2] = { 0x55, 0x00 };
//    uint8_t addr_pipe2[2] = { 0x66, 0x00 };
//    uint8_t addr_pipe3[2] = { 0x99, 0x00 };
//    uint8_t addr_pipe4[2] = { 0xaa, 0x00 };
//    uint8_t addr_pipe5[2] = { 0xcc, 0x00 };
}



void nrf24_coordinator_loop() {

}
