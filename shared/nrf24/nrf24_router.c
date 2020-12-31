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
#include "nrf24_register.h"

#include <iso646.h>
#include <stdio.h>



void nrf24_router_init() {
    nrf24_init();
    nrf24_set_ard(0);                             // TODO: RND [tier+1…15]

//    const uint8_t payload[2] = { ADDR_UP_PING, 0x00 };
//    const uint8_t addr[5] = { 0x00, 0x00, 0x5A, 0xC6, 0x39 };
//    nrf24_set_register_n(TX_ADDR, 5, addr);
//    nrf24_clear_plos_cnt();
//    nrf24_tx(2, payload);
//    nrf24_debug();
//
////    while (!nrf24_tx_done()) {
//        _delay_ms(1000);
//        nrf24_debug();
////    }
//
//    if (nrf24_tx_fail()) {
//        puts("fail");
//        nrf24_flush_tx();
//    } else {
//        puts("success");
//    }
//
//    nrf24_debug();
//
//    while (true);

//    nrf24_set_pipe_address(0, 0x33);
//    nrf24_set_pipe_address(1, 0x55);
//    nrf24_set_pipe_address(2, 0x66);
//    nrf24_set_pipe_address(3, 0x99);
//    nrf24_set_pipe_address(4, 0xAA);
//    nrf24_set_pipe_address(5, 0x00);            // 0xCC
}

//extern volatile nrf24_register_status_u s_nrf24_io_irq_status;

static void scan(uint8_t * const out_fail, uint8_t * const out_re, const uint8_t in_ADDR) {
    const uint8_t payload[1] = { ADDR_UP_PING };
    const uint8_t addr[2] = { 0x00, in_ADDR };
    nrf24_set_register_n(TX_ADDR, 2, addr);
    nrf24_clear_plos_cnt();

    uint8_t re_cnt = 0;
    nrf24_register_observe_tx_u observe_tx;

    for (uint8_t r = 0; r < 17; r++) {
        nrf24_tx(1, payload);

//        while (!s_nrf24_io_irq_status.MAX_RT && !s_nrf24_io_irq_status.TX_DS);
//        nrf24_clear_interrupts();
//        nrf24_flush_tx();

        bool fail = false;
        bool done = false;

        do {
            fail = nrf24_tx_fail();
            if (fail) break;
            done = nrf24_tx_done();
            if (done) break;
        } while (true);

        if (fail) {
            // puts("fail");
            nrf24_flush_tx();
        }

        observe_tx.u8 = nrf24_get_register_1(OBSERVE_TX);

//        printf("%02x\n", observe_tx.u8);
        re_cnt += observe_tx.ARC_CNT;
    }

    *out_fail = observe_tx.PLOS_CNT;
    *out_re = re_cnt;
}



void nrf24_scan() {
    const uint8_t nibbles[] = { 0x3, 0x5, 0x6, 0x9, 0xA, 0xC };
    uint8_t fail_cnt = 0;
    uint8_t re_cnt = 0;

    nrf24_set_ard(0);                                                           // ping can be as quick as possible. we are only sending one byte for ping

    puts("\nTier 0:");
    scan(&fail_cnt, &re_cnt, 0x00);
    printf("0x%02x: %d %d\n", 0x00, fail_cnt, re_cnt);

    puts("\nTier 1:");

    for (int i = 0; i < 6; i++) {
        const uint8_t addr = nibbles[i] << 4 bitor nibbles[i];
        scan(&fail_cnt, &re_cnt, addr);
        printf("0x%02x: %d %d\n", addr, fail_cnt, re_cnt);
    }

    puts("\nTier 2:");

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < i; j++) {
            const uint8_t addr = nibbles[i] << 4 bitor nibbles[j];
            scan(&fail_cnt, &re_cnt, addr);
            printf("0x%02x: %d %d\n", addr, fail_cnt, re_cnt);
        }

        for (int j = i + 1; j < 6; j++) {
            const uint8_t addr = nibbles[i] << 4 bitor nibbles[j];
            scan(&fail_cnt, &re_cnt, addr);
            printf("0x%02x: %d %d\n", addr, fail_cnt, re_cnt);
        }
    }
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
