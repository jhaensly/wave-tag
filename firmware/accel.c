/**
 * @file accel.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "accel.h"
#include "accel_constants.h"
#include "util.h"

static accel_cb_t m_freefall_event_cb;

static void twi_start(uint8_t addr) {
    TWCR = _BV(TWINT)|_BV(TWSTA)|_BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));

    TWDR = addr;
    TWCR = _BV(TWINT)|_BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));
}

static void twi_send_byte(uint8_t data) {
    TWDR = data;
    TWCR = _BV(TWINT)|_BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));
}

#define TWI_START_WRITE(a) twi_start(a)
#define TWI_START_READ(a)  twi_start((a) | 1)

static uint8_t accelReadReg(uint8_t reg) {
    PRR &= ~_BV(PRTWI);

    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);

    TWI_START_READ(ACCEL_I2C_ADDR);

    TWCR = _BV(TWINT)|_BV(TWEN)|(0<<TWEA);		//wait for data; send NACK
    BUSY_UNTIL(TWCR & _BV(TWINT));

    TWCR = _BV(TWSTO)|_BV(TWEN)|_BV(TWINT);		//send stop

    PRR |= _BV(PRTWI);
    return TWDR;
}

static void accelWriteReg(uint8_t reg, uint8_t val) {
    PRR &= ~_BV(PRTWI);

    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);
    twi_send_byte(val);

    TWCR = _BV(TWSTO)|_BV(TWEN)|_BV(TWINT);		//send stop

    PRR |= _BV(PRTWI);
}

error_t accelEnableFreefall(accel_cb_t freefall_event_cb) {
    if (!freefall_event_cb) {
        return ERR_ACCEL_INVALID_ARG;
    }
    m_freefall_event_cb = freefall_event_cb;

    // Enable freefall detect on y; Event latch disable
    accelWriteReg(ACCEL_REG_FF_MT_CFG, 0x10u);

    // Enable freefall detect
    accelWriteReg(ACCEL_REG_CTRL_4, 0x04u);

    // Freefall interrupt to INT2 pin
    accelWriteReg(ACCEL_REG_INT_PIN_MAP, 0x04u);

    // Push pull, active high
    accelWriteReg(ACCEL_REG_CTRL_3, 0x00u);

    // Set threshold. Max = 0x7f, which equals 8 G
    accelWriteReg(ACCEL_REG_FF_MT_THS, 0x13u);

	//Set freefall debounce timeout
	accelWriteReg(ACCEL_REG_FF_MT_COUNT, 0X00);

    // Set ACTIVE bit to wake chip
    accelWriteReg(ACCEL_REG_CTRL_1, 0x01u);

	//Interrupt on INT1. Rising edge.
	EICRA |= _BV(ISC11) | _BV(ISC10);
	EIMSK |= _BV(INT1);

    return ERR_NONE;
}

error_t accelDisable(void) {
    // Put the accelerometer into standby mode
    accelWriteReg(ACCEL_REG_CTRL_1, 0x00);

    // Disable the interrupt
	EIMSK &= ~_BV(INT1);
    return ERR_NONE;
}

error_t accelReadValue(accel_axis_t axis, accel_data_t* data) {
    static const uint8_t axis_reg[] = {
        ACCEL_OUT_X_MSB,
        ACCEL_OUT_Y_MSB,
        ACCEL_OUT_Z_MSB };
    *data = accelReadReg(axis_reg[axis]);

    return ERR_NONE;
}


ISR (INT1_vect) {
	if (m_freefall_event_cb) {
        m_freefall_event_cb();
	}
}
