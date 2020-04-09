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



#define LCD_COLUMNS     16
#define LCD_LINES       2
#define LCD_LINE_WIDTH  40

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



static void write8(uint8_t data) {
    write4(data >> 4);
    write4(data & 0x0F);
}



static void send_command(uint8_t cmd) {
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8(cmd);
}



static void send_data(uint8_t data) {
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8(data);
}



void lcd_putchar(char c) {
    send_data(c);
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
    send_command(CLEAR_DISPLAY);
    _delay_us(LCD_T_LONG);
}



void lcd_io_return_home() {
    send_command(RETURN_HOME);
    _delay_us(LCD_T_LONG);
}



void lcd_io_line(uint8_t line) {
    send_command(SET_DDRAM_ADDR | ((line & 0x1) * 0x40));
}



void lcd_io_set_glyph5x8(uint8_t c, uint8_t data[static const 8]) {
    send_command(SET_CGRAM_ADDR | ((c & 0x7) << 3));

    send_data(data[0]);
    send_data(data[1]);
    send_data(data[2]);
    send_data(data[3]);
    send_data(data[4]);
    send_data(data[5]);
    send_data(data[6]);
    send_data(data[7]);
}



void lcd_io_set_glyph5x11(uint8_t c, uint8_t data[static const 11]) {
    send_command(SET_CGRAM_ADDR | ((c & 0x3) << 4));

    send_data(data[0]);
    send_data(data[1]);
    send_data(data[2]);
    send_data(data[3]);
    send_data(data[4]);
    send_data(data[5]);
    send_data(data[6]);
    send_data(data[7]);
    send_data(data[8]);
    send_data(data[9]);
    send_data(data[10]);
}



void lcd_io_init() {
    BIT_CLR(LCD_CTRL_PORT, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));            // pull control lines low
    BIT_SET(LCD_CTRL_DDR, _BV3(LCD_RS_PIN, LCD_RW_PIN, LCD_E_PIN));             // control lines are output
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

    send_command(FUNCTION_SET | FUNCTION_SET_4BIT | FUNCTION_SET_2LINE | FUNCTION_SET_5x8);
    send_command(DISPLAY_CTRL | DISPLAY_CTRL_DISPLAY | DISPLAY_CTRL_CURSOR | DISPLAY_CTRL_BLINK);

    lcd_io_clear_display();
    
    send_command(ENTRY_MODE | ENTRY_MODE_LTR);

#ifdef LCD_STDOUT
    static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putc, NULL, _FDEV_SETUP_WRITE);
    stdout = &lcd_stdout;
#endif

    lcd_io_line(1);
    puts("Hello, World!");
    lcd_io_line(0);
    puts("Hello, World!");

    _delay_ms(2000);

    lcd_io_line(0);
    puts("0123456789ABCDEF");
    lcd_io_line(1);
    puts("0123456789ABCDEF");




////    puts("0123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA9876543210");
//
//    const char x[] 	= "0123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA98765432100123456789ABCDEFGHIJKLMNOPQRSTabcdefghijjihgfedcbaTSRQPONMLKJIHGFEDCBA9876543210";
//
//    for (uint8_t s = 0; s < 80; s++) {
//        lcd_io_line(0);
//
//        for (uint8_t c = 0; c < 80; c++) {
//            send_data(x[(c + (s%40)) % 80]);
//        }
//
//        //_delay_ms(200);
//    }

//    send_command(ENTRY_MODE | ENTRY_MODE_RTL);
//    send_command(CURSOR_SHIFT | CURSOR_SHIFT_CURSOR | CURSOR_SHIFT_LEFT);
//
//    puts("0123456789");

//    _delay_ms(250);
//
//    for (uint8_t i = 0; i < 80; i++) {
//        send_command(CURSOR_SHIFT | CURSOR_SHIFT_SCREEN | CURSOR_SHIFT_LEFT);
//        _delay_ms(350);
//    }

    return;


//    send_data('H');
//    send_data('e');
//    send_data('l');
//    send_data('l');
//    send_data('o');

    _delay_ms(1000);

    //lcd_io_clear_display();
    lcd_io_line(0);

    send_data('W');
    send_data('o');
    send_data('r');
    send_data('l');
    send_data('d');

    _delay_ms(1000);
    lcd_io_return_home();
//
//    for (uint16_t i = 0; i < 256; i++) {
////        if (i % 16 == 0) {
////            _delay_ms(1000);
////            lcd_io_clear_display();
////        }
//
//        send_data(i);
//        _delay_ms(100);
//    }
    return;

    for (uint8_t i = 0; i < 16; i++) {
        send_data(i);
    }

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
