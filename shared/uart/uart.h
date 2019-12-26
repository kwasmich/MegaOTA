//
//  uart.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 15.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef uart_h
#define uart_h


#include <stdint.h>



typedef void (*uart_callback_t)(const uint8_t in_BYTE);


void uart_init(uart_callback_t const in_UART_CALLBACK, const uint8_t in_OSCCAL);
void uart_loop(void);

void uart_putchar(char c);
uint8_t uart_getchar(void);


#endif /* uart_h */
