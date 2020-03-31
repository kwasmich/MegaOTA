#include "spi.h"

#include "config.h"
#include "macros.h"

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/power.h>
#include <stdio.h>



#define SPI_DDR      DDRB
#define SPI_PORT     PORTB
#define SPI_SS_PIN   PB2
#define SPI_MOSI_PIN PB3
#define SPI_MISO_PIN PB4
#define SPI_SCK_PIN  PB5
// #define SPI_STDOUT



uint8_t spi_exchange(uint8_t data) {
    SPDR = data;
    // _NOP();
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}



#ifdef SPI_STDOUT
static int spi_putc(char c, FILE *stream) {
    if (c == '\n') {
        return spi_putc('\r', stream);
    }

    spi_exchange(c);
    return c;
}
#endif



void spi_init() {
    power_spi_enable();                                                         // enable SPI
    BIT_SET(SPI_DDR, _BV3(SPI_SS_PIN, SPI_MOSI_PIN, SPI_SCK_PIN));              // define SS#, MOSI and SCK as output (MISO is input automatically)
    SPCR = _BV2(SPE, MSTR);                                                     // enable SPI, as Master with f/4
    // SPSR = _BV(SPI2X);                                                       // increase the SPI Master speed to f/2

#ifdef SPI_STDOUT
    static FILE spi_stdout = FDEV_SETUP_STREAM(spi_putc, NULL, _FDEV_SETUP_WRITE);
    stdout = &spi_stdout;
#endif
}



void spi_deinit() {
    power_spi_disable();                                                        // disable SPI
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
