//
//  lcd_io.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 06.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "lcd_io.h"

#include "lcd_command.h"

#include "macros.h"

#include <avr/io.h>
#include <stdio.h>
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

#define LCD_STDOUT


#define LCD_T_SHORT     43                                                      // µs
#define LCD_T_LONG      1539                                                    // µs



static void write4(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    BIT_CLR(LCD_DATA_PORT, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));
    if (data & 0x01) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
//    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB4_PIN));
    if (data & 0x02) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
//    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB5_PIN));
    if (data & 0x04) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
//    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB6_PIN));
    if (data & 0x08) BIT_SET(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
//    else BIT_CLR(LCD_DATA_PORT, _BV(LCD_DB7_PIN));
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_E_PIN));
    _delay_us(LCD_T_SHORT);
}



static void write8(uint8_t data) {
    write4(data >> 4);
    write4(data & 0x0F);
}



void lcd_putchar(char c) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8(c);
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
}



#ifdef LCD_STDOUT
static int lcd_putc(char c, FILE *stream) {
    if (c == '\n') {
        return c;
    }

    if (c == '\r') {
        return c;
    }

    lcd_putchar(c);
    return c;
}
#endif



void lcd_io_clear_display() {
    write8(CLEAR_DISPLAY);
    _delay_us(LCD_T_LONG);
}



void lcd_io_return_home() {
    write8(RETURN_HOME);
    _delay_us(LCD_T_LONG);
}



void lcd_io_line(uint8_t line) {
    write8(SET_DDRAM_ADDR | ((line & 0x1) * 0x40));
}



void lcd_io_set_glyph5x8(uint8_t c, uint8_t data[static const 8]) {
    write8(SET_CGRAM_ADDR | ((c & 0x7) << 3));

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8(data[0]);
    write8(data[1]);
    write8(data[2]);
    write8(data[3]);
    write8(data[4]);
    write8(data[5]);
    write8(data[6]);
    write8(data[7]);
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
}



void lcd_io_set_glyph5x11(uint8_t c, uint8_t data[static const 11]) {
    write8(SET_CGRAM_ADDR | ((c & 0x3) << 4));

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8(data[0]);
    write8(data[1]);
    write8(data[2]);
    write8(data[3]);
    write8(data[4]);
    write8(data[5]);
    write8(data[6]);
    write8(data[7]);
    write8(data[8]);
    write8(data[9]);
    write8(data[10]);
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
}




void lcd_io_init() {
    BIT_CLR(LCD_CTRL_PORT, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));            // pull control lines low
    BIT_SET(LCD_CTRL_DDR, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));             // control lines are output
    BIT_SET(LCD_DATA_PORT, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));   // pull data lines low
    BIT_SET(LCD_DATA_DDR, _BV4(LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN));    // data lines are output in most case

    write4(0x3);
    //_delay_ms(5);   // > 4.1 ms
    write4(0x3);
    //_delay_us(101); // > 100 µs
    write4(0x3);
    //_delay_us(39);
    write4(0x2);
    //_delay_us(39);
    write8(DISPLAY_CTRL | DISPLAY_CTRL_DISPLAY | DISPLAY_CTRL_CURSOR | DISPLAY_CTRL_BLINK);
    write8(FUNCTION_SET | FUNCTION_SET_4BIT | FUNCTION_SET_2LINE | FUNCTION_SET_5x8);
    write8(ENTRY_MODE | ENTRY_MODE_LTR);

    lcd_io_clear_display();

#ifdef LCD_STDOUT
    static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putc, NULL, _FDEV_SETUP_WRITE);
    stdout = &lcd_stdout;
    // printf("%ld %ld %d\n", UBRRH_VALUE, UBRRL_VALUE, USE_2X);
#endif

    lcd_io_line(1);
    puts("Hello, World!");
    lcd_io_line(0);
    puts("Hello, World!");
    return;


//    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
//    write8('H');
//    write8('e');
//    write8('l');
//    write8('l');
//    write8('o');
//    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));

    _delay_ms(1000);

    //lcd_io_clear_display();
    lcd_io_line(0);

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8('W');
    write8('o');
    write8('r');
    write8('l');
    write8('d');
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));

    _delay_ms(1000);
    lcd_io_return_home();
//
//    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
//    for (uint16_t i = 0; i < 256; i++) {
////        if (i % 16 == 0) {
////            _delay_ms(1000);
////            BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
////            lcd_io_clear_display();
////            BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
////        }
//
//        write8(i);
//        _delay_ms(100);
//    }
//    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    return;

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    for (uint8_t i = 0; i < 16; i++) {
        write8(i);
    }
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));

    uint8_t glyph[] = {
        0b11111,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11111,
    };

    lcd_io_set_glyph5x8(4, glyph);

    uint8_t glyph2[] = {
        0b11111,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11111,
        0b01010,
        0b10101,
        0b01010,
    };

    uint8_t glyph3[] = {
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01010,
        0b10101,
        0b01010,
    };

    lcd_io_set_glyph5x11(3, glyph2);
    lcd_io_set_glyph5x11(1, glyph3);

}



void lcd_io_deinit() {

}
