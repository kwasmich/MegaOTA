#include "sst25vfxx.h"

#include "macros.h"
#include "spi.h"

#include <avr/io.h>
#include <util/delay.h>



#define SPI_EEPROM_OP_WRITE_STATUS_REGISTER         0x01
#define SPI_EEPROM_OP_BYTE_PROGRAM                  0x02
#define SPI_EEPROM_OP_READ                          0x03
#define SPI_EEPROM_OP_WRITE_DISABLE                 0x04
#define SPI_EEPROM_OP_READ_STATUS_REGISTER          0x05
#define SPI_EEPROM_OP_WRITE_ENABLE                  0x06
#define SPI_EEPROM_OP_SECTOR_ERASE                  0x20
#define SPI_EEPROM_OP_ENABLE_WRITE_STATUS_REGISTER  0x50
#define SPI_EEPROM_OP_BLOCK_ERASE                   0x52
#define SPI_EEPROM_OP_CHIP_ERASE                    0x60
#define SPI_EEPROM_OP_READ_ID                       0xAB
#define SPI_EEPROM_OP_AAI_PROGRAM                   0xAF

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_SS   PB2

#ifndef SPI_EEPROM_24BIT
#   define SPI_EEPROM_SECTOR_ADDRESS_MASK 0xF000
#   define SPI_EEPROM_BLOCK_ADDRESS_MASK 0x8000
#else
#   define SPI_EEPROM_SECTOR_ADDRESS_MASK 0xFFFFF000
#   define SPI_EEPROM_BLOCK_ADDRESS_MASK 0xFFFF8000
#endif



inline static void spi_start() {
    BIT_CLR(SPI_PORT, _BV(SPI_SS));
}



inline static void spi_stop() {
    BIT_SET(SPI_PORT, _BV(SPI_SS));
}



inline static void spi_eeprom_set_address(Address_t a) {
#if SPI_EEPROM_24BIT == 0
    spi_exchange(0x00);
#else
    spi_exchange((a >> 16) & 0xFF);
#endif
    spi_exchange((a >>  8) & 0xFF);
    spi_exchange((a >>  0) & 0xFF);
}



inline static void spi_eeprom_write_enable() {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_WRITE_ENABLE);
    spi_stop();
}



inline static void spi_eeprom_write_disable() {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_WRITE_DISABLE);
    spi_stop();
}



inline static void spi_eeprom_wait_until_ready() {
    // _delay_ms(100);
    spi_start();
    spi_exchange(SPI_EEPROM_OP_READ_STATUS_REGISTER);
    while (spi_exchange(0x00) & 0x01);
    spi_stop();
}



void spi_eeprom_init() {
    BIT_SET(SPI_DDR, _BV(SPI_SS));  // define SS#, MOSI and SCK as output (MISO is input automatically)
    BIT_SET(SPI_PORT, _BV(SPI_SS)); // keep SS# high and pull it low if the slave is selected
}



void spi_eeprom_read_start(Address_t a) {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_READ);
    spi_eeprom_set_address(a);
}



uint8_t spi_eeprom_read_next() {
    return spi_exchange(0x00);
}



void spi_eeprom_read_stop() {
    spi_stop();
}



uint8_t spi_eeprom_read_byte(Address_t a) {
    spi_eeprom_read_start(a);
    uint8_t data = spi_eeprom_read_next();
    spi_eeprom_read_stop();
    return data;
}



void spi_eeprom_write_start(Address_t a, uint8_t data) {
    spi_eeprom_write_enable();
    spi_start();
    spi_exchange(SPI_EEPROM_OP_AAI_PROGRAM);
    spi_eeprom_set_address(a);
    spi_exchange(data);
    spi_stop();
    //delay(0.020);
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_write_next(uint8_t data) {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_AAI_PROGRAM);
    spi_exchange(data);
    spi_stop();
    //delay(0.020);
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_write_stop() {
    spi_eeprom_write_disable();
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_write_byte(Address_t a, uint8_t data) {
    spi_eeprom_write_enable();
    spi_start();
    spi_exchange(SPI_EEPROM_OP_BYTE_PROGRAM);
    spi_eeprom_set_address(a);
    spi_exchange(data);
    spi_stop();
    //delay(0.020);
    spi_eeprom_wait_until_ready();
}



uint8_t spi_eeprom_manufacturer_id() {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_READ_ID);
    spi_exchange(0x00);
    spi_exchange(0x00);
    spi_exchange(0x00);
    uint8_t id = spi_exchange(0x00);
    spi_stop();
    return id;
}



uint8_t spi_eeprom_device_id() {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_READ_ID);
    spi_exchange(0x00);
    spi_exchange(0x00);
    spi_exchange(0x01);
    uint8_t id = spi_exchange(0x00);
    spi_stop();
    return id;
}



void spi_eeprom_erase_block(Address_t a) {
    spi_eeprom_write_enable();
    spi_start();
    spi_exchange(SPI_EEPROM_OP_BLOCK_ERASE);
    spi_eeprom_set_address(a & SPI_EEPROM_BLOCK_ADDRESS_MASK);
    spi_stop();
    //delay(25);
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_erase_sector(Address_t a) {
    spi_eeprom_write_enable();
    spi_start();
    spi_exchange(SPI_EEPROM_OP_SECTOR_ERASE);
    spi_eeprom_set_address(a & SPI_EEPROM_SECTOR_ADDRESS_MASK);
    spi_stop();
    //delay(25);
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_erase_chip() {
    spi_eeprom_write_enable();
    spi_start();
    spi_exchange(SPI_EEPROM_OP_CHIP_ERASE);
    spi_stop();
    //delay(100);
    spi_eeprom_wait_until_ready();
}



void spi_eeprom_write_protect(bool protect) {
    spi_start();
    spi_exchange(SPI_EEPROM_OP_ENABLE_WRITE_STATUS_REGISTER);
    spi_stop();
    
    spi_start();
    spi_exchange(SPI_EEPROM_OP_WRITE_STATUS_REGISTER);
    
    if (protect) {
        spi_exchange(0x0C);
    } else {
        spi_exchange(0x00);
    }
    
    spi_stop();
}
