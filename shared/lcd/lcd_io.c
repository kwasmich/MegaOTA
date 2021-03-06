//
//  lcd_io.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 06.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "lcd_io.h"
#include "lcd_command.h"

#include "config.h"
#include "macros.h"

#include <avr/io.h>
#include <util/delay.h>



#define LCD_COLUMNS     16
#define LCD_LINES       2
#define LCD_LINE_WIDTH  40

#define LCD_CTRL_DDR    DDRC
#define LCD_CTRL_PORT   PORTC
#define LCD_RS_PIN      PC0
#define LCD_RW_PIN      PC0                                                     // R/W is not used here
#define LCD_E_PIN       PC1

#define LCD_DATA_DDR    DDRC
#define LCD_DATA_PORT   PORTC
#define LCD_DB0_PIN
#define LCD_DB1_PIN
#define LCD_DB2_PIN
#define LCD_DB3_PIN
#define LCD_DB4_PIN     PC2
#define LCD_DB5_PIN     PC3
#define LCD_DB6_PIN     PC4
#define LCD_DB7_PIN     PC5

#define LCD_T_SHORT     43                                                      // µs
#define LCD_T_LONG      1539                                                    // µs

#if defined(LCD_INTERFACE_4_BIT) || defined(LCD_INTERFACE_8_BIT)
#else
#   error LCD interface not defined
#endif



#ifdef LCD_INTERFACE_4_BIT
static void write4(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    if (data & 0x01) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    if (data & 0x02) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    if (data & 0x04) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    if (data & 0x08) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    _delay_us(LCD_T_SHORT);
}
#endif


#ifdef LCD_INTERFACE_8_BIT
#error TDOD
static void write8(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    if (data & 0x01) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    if (data & 0x02) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    if (data & 0x04) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    if (data & 0x08) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    _delay_us(LCD_T_SHORT);
}
#endif



static void write(uint8_t data) {
#ifdef LCD_INTERFACE_4_BIT
    write4(data >> 4);
    write4(data & 0x0F);
#endif
#ifdef LCD_INTERFACE_8_BIT
    write8(data);
#endif
}



void lcd_io_send_command(uint8_t cmd) {
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write(cmd);
}



void lcd_io_send_data(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write(data);
}



void lcd_io_clear_display() {
    lcd_io_send_command(CLEAR_DISPLAY);
    _delay_us(LCD_T_LONG);
}



void lcd_io_return_home() {
    lcd_io_send_command(RETURN_HOME);
    _delay_us(LCD_T_LONG);
}



void lcd_io_init() {
    BIT_CLR(LCD_CTRL_PORT, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));            // pull control lines low
    BIT_SET(LCD_CTRL_DDR, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));             // control lines are output

#ifdef LCD_INTERFACE_4_BIT
    BIT_SET(LCD_DATA_PORT, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));   // pull data lines low
    BIT_SET(LCD_DATA_DDR, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));    // data lines are output in most case

    write4(0x3);
//    _delay_ms(5);   // > 4.1 ms
    write4(0x3);
//    _delay_us(101); // > 100 µs
    write4(0x3);
//    _delay_us(39);
    write4(0x2);
//    _delay_us(39);

    lcd_io_send_command(FUNCTION_SET | FUNCTION_SET_4BIT | FUNCTION_SET_2LINE | FUNCTION_SET_5x8);
#endif

    lcd_io_send_command(DISPLAY_CTRL | DISPLAY_CTRL_DISPLAY | DISPLAY_CTRL_BLINK);
    lcd_io_clear_display();
    lcd_io_send_command(ENTRY_MODE | ENTRY_MODE_LTR);
}




//////    puts("0123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA9876543210");
////
////    const char x[] 	= "0123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA98765432100123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA9876543210";
////
////    for (uint8_t s = 0; s < 80; s++) {
////        lcd_io_line(0);
////
////        for (uint8_t c = 0; c < 80; c++) {
////            send_data(x[(c + (s%40)) % 80]);
////        }
////
////        //_delay_ms(200);
////    }
//
////    send_command(ENTRY_MODE | ENTRY_MODE_RTL);
////    send_command(CURSOR_SHIFT | CURSOR_SHIFT_CURSOR | CURSOR_SHIFT_LEFT);
////
////    puts("0123456789");
//
////    _delay_ms(250);
////
////    for (uint8_t i = 0; i < 80; i++) {
////        send_command(CURSOR_SHIFT | CURSOR_SHIFT_SCREEN | CURSOR_SHIFT_LEFT);
////        _delay_ms(350);
////    }
//
//    return;
//
//
////    send_data('H');
////    send_data('e');
////    send_data('l');
////    send_data('l');
////    send_data('o');
//
//    _delay_ms(1000);
//
//    //lcd_io_clear_display();
//    lcd_io_line(0);
//
//    lcd_io_send_data('W');
//    lcd_io_send_data('o');
//    lcd_io_send_data('r');
//    lcd_io_send_data('l');
//    lcd_io_send_data('d');
//
//    _delay_ms(1000);
//    lcd_io_return_home();
////
////    for (uint16_t i = 0; i < 256; i++) {
//////        if (i % 16 == 0) {
//////            _delay_ms(1000);
//////            lcd_io_clear_display();
//////        }
////
////        send_data(i);
////        _delay_ms(100);
////    }




void lcd_io_deinit() {

}
