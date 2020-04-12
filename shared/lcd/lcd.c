//
//  lcd.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "lcd.h"
#include "lcd_io.h"
#include "lcd_command.h"

#include "config.h"

#include <stdio.h>
#include <string.h>



void lcd_putchar(char c) {
    lcd_io_send_data(c);
}



#ifdef LCD_STDOUT
static int lcd_putc(char c, FILE *stream) {
    if (c == '\n') {
        return c;
    }

    lcd_putchar(c);
    return c;
}
#endif



void lcd_clear_display() {
    lcd_io_clear_display();
}


void lcd_goto_line(uint8_t line) {
    lcd_io_send_command(SET_DDRAM_ADDR | ((line & 0x1) * 0x40));
}



void lcd_set_glyph5x8(uint8_t c, uint8_t data[static const 8]) {
    lcd_io_send_command(SET_CGRAM_ADDR | ((c & 0x7) << 3));

    lcd_io_send_data(data[0]);
    lcd_io_send_data(data[1]);
    lcd_io_send_data(data[2]);
    lcd_io_send_data(data[3]);
    lcd_io_send_data(data[4]);
    lcd_io_send_data(data[5]);
    lcd_io_send_data(data[6]);
    lcd_io_send_data(data[7]);
}



void lcd_set_glyph5x11(uint8_t c, uint8_t data[static const 11]) {
    lcd_io_send_command(SET_CGRAM_ADDR | ((c & 0x3) << 4));

    lcd_io_send_data(data[0]);
    lcd_io_send_data(data[1]);
    lcd_io_send_data(data[2]);
    lcd_io_send_data(data[3]);
    lcd_io_send_data(data[4]);
    lcd_io_send_data(data[5]);
    lcd_io_send_data(data[6]);
    lcd_io_send_data(data[7]);
    lcd_io_send_data(data[8]);
    lcd_io_send_data(data[9]);
    lcd_io_send_data(data[10]);
}



static void test_custom_glyph() {
    for (uint8_t i = 0; i < 8; i++) {
        lcd_io_send_data(i);
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

    lcd_set_glyph5x8(1, glyph);

    memset(glyph, 0, sizeof(glyph));

//    lcd_set_glyph5x8(1, glyph);
//    lcd_set_glyph5x8(2, glyph);
//    lcd_set_glyph5x8(3, glyph);
//    lcd_set_glyph5x8(4, glyph);
//    lcd_set_glyph5x8(5, glyph);
//    lcd_set_glyph5x8(6, glyph);
//    lcd_set_glyph5x8(7, glyph);


    /*

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

    lcd_set_glyph5x11(3, glyph2);
    lcd_set_glyph5x11(1, glyph3);
     */
}



void lcd_init() {
    lcd_io_init();

#ifdef LCD_STDOUT
    static FILE lcd_stdout = FDEV_SETUP_STREAM(lcd_putc, NULL, _FDEV_SETUP_WRITE);
    stdout = &lcd_stdout;
#endif

    puts("READY");
    lcd_goto_line(1);
    //test_custom_glyph();
}



void lcd_deinit() {
    lcd_io_deinit();
}


