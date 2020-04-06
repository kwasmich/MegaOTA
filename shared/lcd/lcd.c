//
//  lcd.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#include "lcd.h"
#include "lcd_io.h"



void lcd_init() {
    lcd_io_init();

    /*
    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8('H');
    write8('e');
    write8('l');
    write8('l');
    write8('o');
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));

    _delay_ms(1000);

    write8(CLEAR_DISPLAY);
    _delay_ms(2);

    BIT_SET(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
    write8('W');
    write8('o');
    write8('r');
    write8('l');
    write8('d');
    BIT_CLR(LCD_CTRL_PORT, _BV(LCD_RS_PIN));
     */
}



void lcd_deinit() {
    lcd_io_deinit();
}
