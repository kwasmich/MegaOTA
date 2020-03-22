//
//  nrf24_io.c
//  TinyRF24
//
//  Created by Michael Kwasnicki on 09.01.18.
//  Copyright © 2018 Michael Kwasnicki. All rights reserved.
//

#include "nrf24_io.h"

#include "config.h"
#include "macros.h"

#include "spi/spi.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>



#ifndef NRF24_CE_PORT
#   define NRF24_CE_PORT  PORTB
#endif
#ifndef NRF24_CE_PIN
#   define NRF24_CE_PIN   PB1
#endif
#ifndef NRF24_CSN_PORT
#   define NRF24_CSN_PORT PORTB
#endif
#ifndef NRF24_CSN_PIN
#   define NRF24_CSN_PIN  PB2
#endif
#ifndef NRF24_IRQ_PORT
#   define NRF24_IRQ_PORT PORTD
#endif
#ifndef NRF24_IRQ_PIN
#   define NRF24_IRQ_PIN  PD2
#endif
#ifndef NRF24_IRQ_INT
#   define NRF24_IRQ_INT  INT0
#endif
#ifndef NRF24_IRQ_VECT
#   define NRF24_IRQ_VECT INT0_vect
#endif

#define NRF24_T_POR 100                                                         // ms
#define NRF24_T_CE 15                                                           // >10 µs



volatile nrf24_register_status_u s_nrf24_io_status;
volatile nrf24_register_status_u s_nrf24_io_irq_status = {};



// try ISR_NAKED with all its consequences
ISR(NRF24_IRQ_VECT) {
    nrf24_register_status_u status = { .MAX_RT = 1, .TX_DS = 1, .RX_DR = 1 };
    nrf24_io_command_1(W_REGISTER xor STATUS, status.u8);
    s_nrf24_io_irq_status.u8 |= s_nrf24_io_status.u8;
}



void nrf24_io_command(uint8_t in_COMMAND) {
    switch (in_COMMAND) {
        case FLUSH_TX:
        case FLUSH_RX:
        case REUSE_TX_PL:
        case NOP:
            break;
        default:
            puts("nrf24_io_command error");
            return;
    }

    BIT_CLR(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
    s_nrf24_io_status.u8 = spi_exchange(in_COMMAND);
    BIT_SET(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
}



uint8_t nrf24_io_command_1(uint8_t in_COMMAND, uint8_t in_DATA) {
    switch (in_COMMAND) {
        case FLUSH_TX:
        case FLUSH_RX:
        case REUSE_TX_PL:
        case NOP:
            puts("nrf24_io_command_1 error");
            return 0;
        default:
            break;
    }

    BIT_CLR(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
    s_nrf24_io_status.u8 = spi_exchange(in_COMMAND);
    uint8_t out_data = spi_exchange(in_DATA);
    BIT_SET(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
    return out_data;
}



void nrf24_io_command_n(uint8_t in_COMMAND, uint8_t in_LENGTH, uint8_t in_out_payload[static const in_LENGTH]) {
    switch (in_COMMAND) {
        case R_RX_PL_WID:
        case FLUSH_TX:
        case FLUSH_RX:
        case REUSE_TX_PL:
        case NOP:
            puts("nrf24_io_command_n error");
            return;
        default:
            break;
    }

    BIT_CLR(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
    s_nrf24_io_status.u8 = spi_exchange(in_COMMAND);

    for (uint8_t i = 0; i < in_LENGTH; i++) {
        if (in_out_payload == NULL) {
            spi_exchange(0xFF);
        } else {
            in_out_payload[i] = spi_exchange(in_out_payload[i]);
        }
    }

    BIT_SET(NRF24_CSN_PORT, _BV(NRF24_CSN_PIN));
}



void nrf24_io_ce_pulse() {
    BIT_SET(NRF24_CE_PORT, _BV(NRF24_CE_PIN));
    _delay_us(NRF24_T_CE);
    BIT_CLR(NRF24_CE_PORT, _BV(NRF24_CE_PIN));
}



void nrf24_io_ce_hi() {
    BIT_SET(NRF24_CE_PORT, _BV(NRF24_CE_PIN));
}



void nrf24_io_ce_lo() {
    BIT_CLR(NRF24_CE_PORT, _BV(NRF24_CE_PIN));
}



void nrf24_io_init() {
    uint8_t sreg = SREG;                                                        // store previous interrupt status
    cli();                                                                      // disable interrupts
#if NRF24_IRQ_INT == INT0
    BIT_SET(EICRA, _BV(ISC01));                                                 // interrupt on falling edge of INT0
    BIT_CLR(EICRA, _BV(ISC00));
#elif NRF24_IRQ_INT == INT1
    BIT_SET(EICRA, _BV(ISC11));                                                 // interrupt on falling edge of INT1
    BIT_CLR(EICRA, _BV(ISC10));
#else
#   error "no interrupt defined!"
#endif
    
    BIT_SET(EIMSK, _BV(NRF24_IRQ_INT));                                         // enable interrupt on INTx
    SREG = sreg;                                                                // restore previous interrupt status

    BIT_CLR(DDRD, _BV(NRF24_IRQ_PIN));                                          // IRQ is input
    BIT_SET(DDRB, _BV2(NRF24_CE_PIN, NRF24_CSN_PIN));                           // CE and CSN are outputs
    BIT_SET(PORTB, _BV(NRF24_CSN_PIN));                                         // keep CSN up unless we are sending data to nRF
    BIT_CLR(PORTB, _BV(NRF24_CE_PIN));                                          // keep CE low unless we want to use the radio
    _delay_ms(NRF24_T_POR);                                                     // wait for nRF24L01+ Oscillator to sattle (T_POR Power on reset)
}
