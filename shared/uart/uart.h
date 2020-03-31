//
//  uart.h
//  MegaRF24
//
//  Created by Michael Kwasnicki on 15.01.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#ifndef uart_h
#define uart_h



#include <stdbool.h>
#include <stdint.h>



// synchronous interface
void uart_init(uint8_t in_OSCCAL);
void uart_deinit(void);
void uart_putchar(char c);
uint8_t uart_getchar(void);



// asynchronous interface
void uart_init_async(uint8_t in_OSCCAL);
void uart_deinit_async(void);
// void uart_putchar_async(char c);
bool uart_getchar_async(char * const out_c);



#endif /* uart_h */
