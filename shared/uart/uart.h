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



// synchronous interface
void uart_init(uint8_t in_OSCCAL);
void uart_putchar(char c);
uint8_t uart_getchar(void);



// asynchronous interface
typedef void (uart_callback_t)(uint8_t in_BYTE);
void uart_init_async(uart_callback_t * const in_UART_CALLBACK, uint8_t in_OSCCAL);
void uart_loop_async(void);
// void uart_putchar_async(char c);



#endif /* uart_h */
