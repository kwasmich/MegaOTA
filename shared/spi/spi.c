#include "spi.h"

#include "config.h"
#include "macros.h"

#include <avr/cpufunc.h> 
#include <avr/io.h>



#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI PB3
#define MISO PB4
#define SCK  PB5



void spi_init() {
    BIT_CLR(PRR, _BV(PRSPI));          // enable SPI
    BIT_SET(SPI_DDR, _BV2(MOSI, SCK)); // define SS#, MOSI and SCK as output (MISO is input automatically)
    SPCR = _BV2(SPE, MSTR);            // enable SPI, as Master with f/4
}



uint8_t spi_exchange(uint8_t data) {
    SPDR = data;
    // _NOP();
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}



/**
 * failed attempt to decrease size 
 * 
uint8_t spi_read() {
    //SPDR = 0x00;
    //_NOP();
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}



void spi_write(uint8_t data) {
    SPDR = data;
    //_NOP();
    loop_until_bit_is_set(SPSR, SPIF);
}
*/