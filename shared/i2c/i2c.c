#include "i2c.h"

#include "config.h"
#include "macros.h"

#include <avr/power.h>
#include <util/twi.h>



#ifndef I2C_SPEED
#   define I2C_SPEED 400000
#endif

#define TWI_PORT PORTC
#define SDA PC4
#define SCL PC5



void i2c_init() {
  power_twi_enable();                                                           // enable TWI
  BIT_SET(TWI_PORT, _BV2(SDA, SCL));                                            // pullup SDA & SCL
  TWSR = 0;                                                                     // Prescaler = 1
  TWBR = (F_CPU / (2 * I2C_SPEED) - 8) / 1;                                     // set bitrate
}



void i2c_deinit() {
    power_twi_disable();                                                        // disable TWI
}



bool i2c_start() {
    TWCR = _BV3(TWINT, TWSTA, TWEN);                                // Send START condition.
    loop_until_bit_is_set(TWCR, TWINT);                             // Wait for TWINT Flag set. This indicates that the START condition has been transmitted.
    return (TW_STATUS == TW_START) || (TW_STATUS == TW_REP_START);  // Check value of TWI Status Register. Mask prescaler bits. If status different from (REP_)START go to ERROR.
}



void i2c_stop() {
    TWCR = _BV3(TWINT, TWEN, TWSTO);  // Transmit STOP condition.
}



bool i2c_address(uint8_t SLA, bool R) {
    const uint8_t TW_SLA_ACK = (R) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK;
    TWDR = (SLA << 1) | R;              // Load SLA_W into TWDR Register.
    TWCR = _BV2(TWINT, TWEN);           // Clear TWINT bit in TWCR to start transmission of address.
    loop_until_bit_is_set(TWCR, TWINT); // Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
    return TW_STATUS == TW_SLA_ACK;     // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR.
}



bool i2c_data(uint8_t DATA, bool R, bool ACK) {
    const uint8_t TW_DATA_ACK = (R) ? TW_MR_DATA_ACK : TW_MT_DATA_ACK;
    const uint8_t TW_DATA_NACK = (R) ? TW_MR_DATA_NACK : TW_MT_DATA_NACK;
    const uint8_t TW_DATA_ACKNACK = (R & !ACK) ? TW_DATA_NACK : TW_DATA_ACK;
    TWDR = DATA;                          // Load DATA into TWDR Register.

    if (R && ACK) {
        TWCR = _BV3(TWINT, TWEN, TWEA);   // Clear TWINT bit in TWCR to start transmission of data and ACK.
    } else {
        TWCR = _BV2(TWINT, TWEN);         // Clear TWINT bit in TWCR to start transmission of data.
    }

    loop_until_bit_is_set(TWCR, TWINT);   // Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    return TW_STATUS == TW_DATA_ACKNACK;  // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR.
}



void i2c_send_byte(uint8_t SLA, uint8_t DATA) {
    i2c_start();
    i2c_address(SLA, false);
    i2c_data(DATA, false, false);
    i2c_stop();
}



void i2c_send_block(uint8_t SLA, uint8_t *DATA, uint8_t CNT) {
    bool status;
    status = i2c_start();
    status = i2c_address(SLA, false);

    for (uint8_t i = 0; i < CNT; i++) {
        status = i2c_data(DATA[i], false, false);
    }

    i2c_stop();
}
