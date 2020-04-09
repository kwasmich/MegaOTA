//
//  lcd_command.h
//  MegaOTA
//
//  Created by Michael Kwasnicki on 05.04.20.
//  Copyright © 2020 Michael Kwasnicki. All rights reserved.
//

#ifndef lcd_command_h
#define lcd_command_h

typedef enum {
    CLEAR_DISPLAY               = 0x01,
    RETURN_HOME                 = 0x02,
    ENTRY_MODE                  = 0x04,
    DISPLAY_CTRL                = 0x08,
    CURSOR_SHIFT                = 0x10,
    FUNCTION_SET                = 0x20,
    SET_CGRAM_ADDR              = 0x40,
    SET_DDRAM_ADDR              = 0x80,

    ENTRY_MODE_LTR              = 0x02,
    ENTRY_MODE_RTL              = 0x00,
    ENTRY_MODE_SH               = 0x01,

    DISPLAY_CTRL_DISPLAY        = 0x04,
    DISPLAY_CTRL_CURSOR         = 0x02,
    DISPLAY_CTRL_BLINK          = 0x01,

    CURSOR_SHIFT_SCREEN         = 0x08,
    CURSOR_SHIFT_CURSOR         = 0x00,
    CURSOR_SHIFT_RIGHT          = 0x04,
    CURSOR_SHIFT_LEFT           = 0x00,

    FUNCTION_SET_8BIT           = 0x10,
    FUNCTION_SET_4BIT           = 0x00,
    FUNCTION_SET_2LINE          = 0x08,
    FUNCTION_SET_1LINE          = 0x00,
    FUNCTION_SET_5x11           = 0x04,                                         // only in combination with 1LINE mode
    FUNCTION_SET_5x8            = 0x00
} lcd_command_e;



typedef enum {
    INCREMENT = 0x00,
    DECREMENT = 0x01
} lcd_dir_e;


#endif /* lcd_command_h */
