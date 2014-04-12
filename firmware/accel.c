/**
 * @file accel.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "accel.h"
#include "accel_constants.h"
#include "util.h"

/* Private variables */
static void (* m_freefall_event_cb)(void);


/* Private function declarations */
static void twiStart(uint8_t addr);
static void twiSendByte(uint8_t data);
static uint8_t accelReadReg(uint8_t reg);
static void accelWriteReg(uint8_t reg, uint8_t val);


/* Function definitions */
static void twiStart(uint8_t addr)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));

    TWDR = addr;
    TWCR = _BV(TWINT) | _BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));
}

static void twiSendByte(uint8_t data)
{
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);
    BUSY_UNTIL(TWCR & _BV(TWINT));
}

static uint8_t accelReadReg(uint8_t reg)
{
    /* Enable the TWI peripheral clock */
    PRR &= ~_BV(PRTWI);

    /* Select the specified register */
    twiStart(ACCEL_I2C_ADDR | TW_WRITE);
    twiSendByte(reg);

    /* Restart as master receiver */
    twiStart(ACCEL_I2C_ADDR | TW_READ);

    /* Wait for data; send NACK */
    TWCR = _BV(TWINT) | _BV(TWEN) | (0<<TWEA);
    BUSY_UNTIL(TWCR & _BV(TWINT));

    /* Send the "stop" signal */
    TWCR = _BV(TWSTO) | _BV(TWEN) | _BV(TWINT);
    BUSY_WHILE(TWCR & _BV(TWSTO));

    /* Read the received byte */
    uint8_t val = TWDR;

    /* Disable the peripheral clock */
    PRR |= _BV(PRTWI);

    return val;
}

static void accelWriteReg(uint8_t reg, uint8_t val)
{
    /* Enable the TWI peripheral clock */
    PRR &= ~_BV(PRTWI);

    /* Write the register, then the address */
    twiStart(ACCEL_I2C_ADDR | TW_WRITE);
    twiSendByte(reg);
    twiSendByte(val);

    /* Send stop */
    TWCR = _BV(TWSTO) | _BV(TWEN) | _BV(TWINT);
    BUSY_WHILE(TWCR & _BV(TWSTO));

    /* Disable the peripheral clock */
    PRR |= _BV(PRTWI);
}

enum error_t accelEnableFreefall(void (* event_cb)(void))
{
    if (!event_cb) {
        return ERR_ACCEL_INVALID_ARG;
    }

    m_freefall_event_cb = event_cb;

    /* Enable freefall detect on y; Event latch disable */
    accelWriteReg(ACCEL_REG_FF_MT_CFG, 0x10u);

    /* Enable freefall detect */
    accelWriteReg(ACCEL_REG_CTRL_4, 0x04u);

    /* Freefall interrupt to INT2 pin */
    accelWriteReg(ACCEL_REG_INT_PIN_MAP, 0x04u);

    /* Push pull, active high */
    accelWriteReg(ACCEL_REG_CTRL_3, 0x00u);

    /* Set threshold. Max = 0x7f, which equals 8 G */
    accelWriteReg(ACCEL_REG_FF_MT_THS, 0x13u);

	/* Set freefall debounce timeout */
	accelWriteReg(ACCEL_REG_FF_MT_COUNT, 0X00);

    /* Set ACTIVE bit to wake chip */
    accelWriteReg(ACCEL_REG_CTRL_1, 0x01u);

	/* Interrupt on INT1. Rising edge. */
	EICRA |= _BV(ISC11) | _BV(ISC10);
	EIMSK |= _BV(INT1);

    return ERR_NONE;
}

enum error_t accelDisable(void)
{
    /* Put the accelerometer into standby mode */
    accelWriteReg(ACCEL_REG_CTRL_1, 0x00);

    /* Disable the interrupt */
	EIMSK &= ~_BV(INT1);
    return ERR_NONE;
}

enum error_t accelReadValue(enum accel_axis_t axis, int8_t* data)
{
    static const uint8_t axis_reg[] = {
        ACCEL_OUT_X_MSB,
        ACCEL_OUT_Y_MSB,
        ACCEL_OUT_Z_MSB };
    *data = accelReadReg(axis_reg[axis]);

    return ERR_NONE;
}

ISR (INT1_vect)
{
	if (m_freefall_event_cb) {
        m_freefall_event_cb();
	}
}
