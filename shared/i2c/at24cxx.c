#include "at24cxx.h"

#include "config.h"
#include "i2c.h"

#include <util/twi.h>



#ifndef I2C_EEPROM_PAGE_SIZE
#   define I2C_EEPROM_PAGE_SIZE 128
#endif

#ifndef I2C_EEPROM_ADDRESS
#   define I2C_EEPROM_ADDRESS 0x50
#endif

#define I2C_EEPROM_PAGE_SIZE_MASK (I2C_EEPROM_PAGE_SIZE - 1)



uint8_t i2c_eeprom_read_byte(uint16_t ADDR) {
    bool status;
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, false);
    status = i2c_data(ADDR >> 8, false, false);
    status = i2c_data(ADDR & 0xFF, false, false);
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, true);
    status = i2c_data(0x00, true, false);
    i2c_stop();
    return TWDR;
}



uint8_t i2c_eeprom_read_next_byte() {
    bool status;
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, true);
    status = i2c_data(0x00, true, false);
    i2c_stop();
    return TWDR;
}



void i2c_eeprom_read_block(uint8_t *out, uint8_t CNT, uint16_t ADDR) {
    bool status;
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, false);
    status = i2c_data(ADDR >> 8, false, false);
    status = i2c_data(ADDR & 0xFF, false, false);
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, true);

    for (uint8_t i = 0; i < CNT; i++) {
        status = i2c_data(0x00, true, ((i + 1) < CNT));
        out[i] = TWDR;
    }

    i2c_stop();
}



bool i2c_eeprom_write_byte(uint16_t ADDR, uint8_t DATA) {
    bool status;
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, false);
    status = i2c_data(ADDR >> 8, false, false);
    status = i2c_data(ADDR & 0xFF, false, false);
    status = i2c_data(DATA, false, false);
    i2c_stop();
}



bool i2c_eeprom_write_block(uint16_t ADDR, uint8_t *DATA, uint8_t CNT) {
    if (((ADDR & I2C_EEPROM_PAGE_SIZE_MASK) + CNT) > I2C_EEPROM_PAGE_SIZE) {
        return false; // "Error: page wrap around!"
    }

    bool status;
    status = i2c_start();
    status = i2c_address(I2C_EEPROM_ADDRESS, false);
    status = i2c_data(ADDR >> 8, false, false);
    status = i2c_data(ADDR & 0xFF, false, false);

    for (uint8_t i = 0; i < CNT; i++) {
        status = i2c_data(DATA[i], false, false);
    }
    
    i2c_stop();
}
