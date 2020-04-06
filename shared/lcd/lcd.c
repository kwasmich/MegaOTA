//
//  lcd.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "lcd.h"
#include "lcd_command.h"

#include "macros.h"

#include <avr/io.h>
#include <util/delay.h>



#define LCD_CTRL_DDR    DDRB
#define LCD_CTRL_PORT   PORTB
#define LCD_RS_PIN      PB4
#define LCD_RW_PIN      PB2
#define LCD_E_PIN       PB3

#define LCD_DATA_DDR    DDRD
#define LCD_DATA_PORT   PORTD
#define LCD_DB0_PIN
#define LCD_DB1_PIN
#define LCD_DB2_PIN
#define LCD_DB3_PIN
#define LCD_DB4_PIN     PD5
#define LCD_DB5_PIN     PD4
#define LCD_DB6_PIN     PD3
#define LCD_DB7_PIN     PD2



static void write4(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    BIT_CLR(LCD_DATA_PORT, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));
    if (data & 0x01) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    if (data & 0x02) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    if (data & 0x04) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    if (data & 0x08) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_E_PIN));
}



static void write8(uint8_t data) {
    write4(data >> 4);
    _delay_us(39);
    write4(data & 0x0F);
    _delay_us(39);
}



void lcd_init() {
    BIT_CLR(LCD_CTRL_PORT, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));            // pull control lines low
    BIT_SET(LCD_CTRL_DDR, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));             // control lines are output
    BIT_SET(LCD_DATA_PORT, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));   // pull data lines low
    BIT_SET(LCD_DATA_DDR, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));    // data lines are output in most case

    uint8_t data[] = {
        0x3, 0x3, 0x3, 0x2, 0x2, 0x8, 0x0, 0xc, 0x0, 0x1, 0x0, 0x6
    };

//    for (uint8_t i = 0; i < 12; i++) {
//        write4(data[i]);
//        _delay_us(125);
//    }

    write4(0x3);
    _delay_ms(5);   // > 4.1 ms
    write4(0x3);
    _delay_us(101); // > 100 µs
    write4(0x3);
    _delay_us(39);
    write4(0x2);
    _delay_us(39);
    write8(DISPLAY_CTRL | DISPLAY_CTRL_DISPLAY | DISPLAY_CTRL_CURSOR | DISPLAY_CTRL_BLINK);
    _delay_us(39);
    write8(FUNCTION_SET | FUNCTION_SET_4BIT | FUNCTION_SET_2LINE | FUNCTION_SET_5x11);
    _delay_us(39);
    write8(ENTRY_MODE | ENTRY_MODE_LTR);
    _delay_us(39);
    write8(CLEAR_DISPLAY);
    _delay_ms(2);

    _delay_ms(1000);

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8('H');
    write8('e');
    write8('l');
    write8('l');
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
}



void lcd_deinit() {

}
