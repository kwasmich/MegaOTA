//
//  nrf24.c
//  MegaRF24
//
//  Created by Michael Kwasnicki on 04.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include "nrf24.h"

#include "nrf24_io.h"

#include <iso646.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>



#define NRF24_T_PD2STBY 4.5                                                     // ms



//39C65A address prefix


nrf24_register_config_u s_nrf24_config;



static uint8_t nrf24_get_register_1(const uint8_t in_REGISTER) {
    return nrf24_io_command_1(R_REGISTER xor in_REGISTER, 0x00);
}



static void nrf24_set_register_1(const uint8_t in_REGISTER, const uint8_t in_DATA) {
    nrf24_io_command_1(W_REGISTER xor in_REGISTER, in_DATA);
}



static void nrf24_get_register_n(const uint8_t in_REGISTER, const uint8_t in_LENGTH, uint8_t out_DATA[static const in_LENGTH]) {
    nrf24_io_command_n(R_REGISTER xor in_REGISTER, in_LENGTH, out_DATA);
}



static void nrf24_set_register_n(const uint8_t in_REGISTER, const uint8_t in_LENGTH, const uint8_t in_DATA[static const in_LENGTH]) {
    uint8_t data[in_LENGTH];
    memcpy(data, in_DATA, in_LENGTH);
    nrf24_io_command_n(W_REGISTER xor in_REGISTER, in_LENGTH, data);
}



static void pwr_up() {
    s_nrf24_config.u8 = nrf24_get_register_1(CONFIG);
    
    if (!s_nrf24_config.PWR_UP) {
        s_nrf24_config.PWR_UP = 1;
        nrf24_set_register_1(CONFIG, s_nrf24_config.u8);
        _delay_ms(NRF24_T_PD2STBY);
    }
}



static void pwr_down() {
    s_nrf24_config.u8 = nrf24_get_register_1(CONFIG);
    
    if (s_nrf24_config.PWR_UP) {
        s_nrf24_config.PWR_UP = 0;
        nrf24_set_register_1(CONFIG, s_nrf24_config.u8);
    }
}



static void reset_plos_cnt() {
    uint8_t ch = nrf24_get_register_1(RF_CH);
    nrf24_set_register_1(RF_CH, ch);
}



static void nrf24_clear_interrupts() {
    nrf24_register_status_u status = { .MAX_RT = 1, .TX_DS = 1, .RX_DR = 1 };
    nrf24_io_command_1(W_REGISTER xor STATUS, status.u8);
}

















void nrf24_rx() {
    uint8_t length = nrf24_get_register_1(R_RX_PL_WID);
    
    if (length > 32) {
        nrf24_io_command(FLUSH_RX);
    } else {
        
    }
    
    
    //    config.PWR_UP = 1;
    //    config.PRIM_RX = 1;
    //    nrfSetRegister(CONFIG, config.u8);
    //    BIT_SET(PORTB, _BV(NRF24_CE));                                              // start listening
    //
    //    BIT_CLR(PORTB, _BV(NRF24_CE));                                              // stop listening
}



void nrf24_tx(uint8_t length, uint8_t pipe, uint8_t payload[static const length]) {
    s_nrf24_config.PWR_UP = 1;
    s_nrf24_config.PRIM_RX = 0;
    nrf24_set_register_1(CONFIG, s_nrf24_config.u8);
    
    
    // to
    // payload
    
    // while tx fifo
    nrf24_io_ce_pulse();
}



void nrf24_init() {
    uint8_t addr[5] = { 0x00, 0x00, 0x5A, 0xC6, 0x39 }; // prefix + addr = 39C65A 0000
    nrf24_register_config_u config = { .CRCO = 1, .EN_CRC = 1 };
    // nrf24_register_en_aa_u en_aa = { .ENAA_P0 = 1, .ENAA_P1 = 1, .ENAA_P2 = 1, .ENAA_P3 = 1, .ENAA_P4 = 1, .ENAA_P5 = 1, };
    nrf24_register_en_rxaddr_u en_rxaddr = { .ERX_P0 = 1 };
    // nrf24_register_setup_aw_u setup_aw = { .AW = 0b11 };
    nrf24_register_setup_retr_u setup_retr = { .ARC = 15, .ARD = 1 };
    nrf24_register_rf_ch_u rf_ch = { .RF_CH = 0x2E };
    nrf24_register_rf_setup_u rf_setup = { .RF_DR_LOW = 0, .RF_DR_HIGH = 0, .RF_PWR = 0b00 };
    nrf24_register_rx_pw_u rx_pw = { .RX_PW = 32 };                             // is this required for dynamic payload?
    nrf24_register_dynpd_u dynpd = { .DPL_P0 = 1, .DPL_P1 = 1, .DPL_P2 = 1, .DPL_P3 = 1, .DPL_P4 = 1, .DPL_P5 = 1 };
    nrf24_register_feature_u feature = { .EN_DPL = 1, .EN_ACK_PAY = 1, .EN_DYN_ACK = 1 };

    nrf24_set_register_1(CONFIG, config.u8);
    // nrf24_set_register_1(EN_AA, en_aa.u8);
    nrf24_set_register_1(EN_RXADDR, en_rxaddr.u8);
    // nrf24_set_register_1(SETUP_AW, setup_aw.u8);
    nrf24_set_register_1(SETUP_RETR, setup_retr.u8);
    nrf24_set_register_1(RF_CH, rf_ch.u8);
    nrf24_set_register_1(RF_SETUP, rf_setup.u8);
    nrf24_set_register_n(RX_ADDR_P0, 5, addr);
    nrf24_set_register_n(RX_ADDR_P1, 5, addr);
    nrf24_set_register_n(TX_ADDR, 5, addr);
    nrf24_set_register_1(RX_PW_P0, rx_pw.u8);
    nrf24_set_register_1(RX_PW_P1, rx_pw.u8);
    nrf24_set_register_1(RX_PW_P2, rx_pw.u8);
    nrf24_set_register_1(RX_PW_P3, rx_pw.u8);
    nrf24_set_register_1(RX_PW_P4, rx_pw.u8);
    nrf24_set_register_1(RX_PW_P5, rx_pw.u8);
    nrf24_set_register_1(DYNPD, dynpd.u8);
    nrf24_set_register_1(FEATURE, feature.u8);
}



void nrf24_loop() {
    if (s_nrf24_io_irq) {
        
    }
}



void nrf24_debug() {
    static const char* name[] = { "CONFIG", "EN_AA", "EN_RXADDR", "SETUP_AW",
        "SETUP_RETR", "RF_CH", "RF_SETUP", "STATUS", "OBSERVE_TX", "RPD",
        "RX_ADDR_P0", "RX_ADDR_P1", "RX_ADDR_P2", "RX_ADDR_P3", "RX_ADDR_P4",
        "RX_ADDR_P5", "TX_ADDR", "RX_PW_P0", "RX_PW_P1", "RX_PW_P2", "RX_PW_P3",
        "RX_PW_P4 ", "RX_PW_P5", "FIFO_STATUS", "(reserved)", "(reserved)",
        "(reserved)", "(reserved)", "DYNPD", "FEATURE", "(reserved)",
        "(reserved)"};
    static const uint32_t len = 0x10C00;

    for (uint8_t i = 0; i < 0x20; i++) {
        uint8_t u8;
        uint8_t u85[5];

        if ((len >> i) & 0x1) {
            nrf24_get_register_n(i, 5, u85);
            printf("%11s: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", name[i], u85[0], u85[1], u85[2], u85[3], u85[4]);
        } else {
            u8 = nrf24_get_register_1(i);
            printf("%11s: 0x%02X\n", name[i], u8);
        }
    }
}
