//
//  nrf24_register.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 12.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef nrf24_register_h
#define nrf24_register_h


#include <stdint.h>


typedef enum {
    CONFIG      = 0x00,
    EN_AA       = 0x01,
    EN_RXADDR   = 0x02,
    SETUP_AW    = 0x03,
    SETUP_RETR  = 0x04,
    RF_CH       = 0x05,
    RF_SETUP    = 0x06,
    STATUS      = 0x07,
    OBSERVE_TX  = 0x08,
    RPD         = 0x09,
    RX_ADDR_P0  = 0x0A,  // 5 byte
    RX_ADDR_P1  = 0x0B,  // 5 byte
    RX_ADDR_P2  = 0x0C,
    RX_ADDR_P3  = 0x0D,
    RX_ADDR_P4  = 0x0E,
    RX_ADDR_P5  = 0x0F,
    TX_ADDR     = 0x10,  // 5 byte
    RX_PW_P0    = 0x11,
    RX_PW_P1    = 0x12,
    RX_PW_P2    = 0x13,
    RX_PW_P3    = 0x14,
    RX_PW_P4    = 0x15,
    RX_PW_P5    = 0x16,
    FIFO_STATUS = 0x17,
    // ... reserved
    DYNPD       = 0x1C,
    FEATURE     = 0x1D
} nrf24_register_e;


typedef enum {
    R_REGISTER         = 0x00,  // 1-5
    W_REGISTER         = 0x20,  // 1-5
    R_RX_PL_WID        = 0x60,  // 1
    R_RX_PAYLOAD       = 0x61,  // 1-32
    W_TX_PAYLOAD       = 0xA0,  // 1-32
    W_ACK_PAYLOAD      = 0xA8,  // 1-32
    W_TX_PAYLOAD_NOACK = 0xB0,  // 1-32
    FLUSH_TX           = 0xE1,  // 0
    FLUSH_RX           = 0xE2,  // 0
    REUSE_TX_PL        = 0xE3,  // 0
    NOP                = 0xFF   // 0
} nrf24_command_e;


typedef union {
    struct {
        uint8_t PRIM_RX: 1;
        uint8_t PWR_UP: 1;
        uint8_t CRCO: 1;
        uint8_t EN_CRC: 1;
        uint8_t MASK_MAX_RT: 1;
        uint8_t MASK_TX_DS: 1;
        uint8_t MASK_RX_DR: 1;
    };

    uint8_t u8;
} nrf24_register_config_u;


typedef union {
    struct {
        uint8_t ENAA_P0: 1;
        uint8_t ENAA_P1: 1;
        uint8_t ENAA_P2: 1;
        uint8_t ENAA_P3: 1;
        uint8_t ENAA_P4: 1;
        uint8_t ENAA_P5: 1;
    };

    uint8_t u8;
} nrf24_register_en_aa_u;


typedef union {
    struct {
        uint8_t ERX_P0: 1;
        uint8_t ERX_P1: 1;
        uint8_t ERX_P2: 1;
        uint8_t ERX_P3: 1;
        uint8_t ERX_P4: 1;
        uint8_t ERX_P5: 1;
    };

    uint8_t u8;
} nrf24_register_en_rxaddr_u;


typedef union {
    struct {
        uint8_t AW: 2;
    };

    uint8_t u8;
} nrf24_register_setup_aw_u;


typedef union {
    struct {
        uint8_t ARC: 4;
        uint8_t ARD: 4;
    };

    uint8_t u8;
} nrf24_register_setup_retr_u;


typedef union {
    struct {
        uint8_t RF_CH: 7;
    };

    uint8_t u8;
} nrf24_register_rf_ch_u;


typedef union {
    struct {
        uint8_t _1: 1;
        uint8_t RF_PWR: 2;
        uint8_t RF_DR_HIGH: 1;
        uint8_t PLL_LOCK: 1;
        uint8_t RF_DR_LOW: 1;
        uint8_t _2: 1;
        uint8_t CONT_WAVE: 1;
    };

    uint8_t u8;
} nrf24_register_rf_setup_u;


typedef union {
    struct {
        uint8_t TX_FULL: 1;
        uint8_t RX_P_NO: 3;
        uint8_t MAX_RT: 1;
        uint8_t TX_DS: 1;
        uint8_t RX_DR: 1;
    };

    uint8_t u8;
} nrf24_register_status_u;


typedef union {
    struct {
        uint8_t ARC_CNT: 4;
        uint8_t PLOS_CNT: 4;
    };

    uint8_t u8;
} nrf24_register_observe_tx_u;


typedef union {
    struct {
        uint8_t RPD: 1;
    };

    uint8_t u8;
} nrf24_register_rpd_u;


typedef union {
    struct {
        uint8_t RX_PW: 6;
    };

    uint8_t u8;
} nrf24_register_rx_pw_u;


typedef union {
    struct {
        uint8_t RX_EMPTY: 1;
        uint8_t RX_FULL: 1;
        uint8_t _: 2;
        uint8_t TX_EMPTY: 1;
        uint8_t TX_FULL: 1;
        uint8_t TX_REUSE: 1;
    };

    uint8_t u8;
} nrf24_register_fifi_status_u;


typedef union {
    struct {
        uint8_t DPL_P0: 1;
        uint8_t DPL_P1: 1;
        uint8_t DPL_P2: 1;
        uint8_t DPL_P3: 1;
        uint8_t DPL_P4: 1;
        uint8_t DPL_P5: 1;
    };

    uint8_t u8;
} nrf24_register_dynpd_u;


typedef union {
    struct {
        uint8_t EN_DYN_ACK: 1;
        uint8_t EN_ACK_PAY: 1;
        uint8_t EN_DPL: 1;
    };
    
    uint8_t u8;
} nrf24_register_feature_u;


#endif /* nrf24_register_h */
