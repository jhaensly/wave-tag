/**
 * @file accel.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 */

#include <avr/io.h>

#include "accel.h"
#include "accel_constants.h"
#include "util.h"

static void twi_start(uint8_t addr) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));

    TWDR = addr;
    TWCR = (1<<TWINT)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));
}

static void twi_send_byte(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));
}

#define TWI_START_WRITE(a) twi_start(a)
#define TWI_START_READ(a)  twi_start((a) | 1)

static uint8_t accelReadReg(uint8_t reg) {
    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);

    TWI_START_READ(ACCEL_I2C_ADDR);

    TWCR = (1<<TWINT)|(1<<TWEN)|(0<<TWEA);		//wait for data; send NACK
    BUSY_UNTIL(TWCR & (1<<TWINT));

    TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);		//send stop
    return TWDR;
}

static void accelWriteReg(uint8_t reg, uint8_t val) {
    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);
    twi_send_byte(val);

    TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);		//send stop
}

error_t accelConfigFreefall() {
    // Enable freefall detect on y; Event latch disable
    accelWriteReg(ACCEL_REG_FF_MT_CFG, 0x50u);

    // Enable freefall detect
    accelWriteReg(ACCEL_REG_CTRL_4, 0x04u);

    // Freefall interrupt to INT2 pin
    accelWriteReg(ACCEL_REG_INT_PIN_MAP, 0x04u);

    // Push pull, active high
    accelWriteReg(ACCEL_REG_CTRL_3, 0x00u);

    // Set threshold. Max = 0x7f, which equals 8 G
    accelWriteReg(ACCEL_REG_FF_MT_THS, 0x0fu);

	//Set freefall debounce timeout
	accelWriteReg(ACCEL_REG_FF_MT_COUNT, 0X01);

    // Set ACTIVE bit to wake chip
    accelWriteReg(ACCEL_REG_CTRL_1, 0x01u);

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
