/**
 * @file accel_constants.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 21 Jan 2014
 *
 * Helpful constants for accessing the Freescale MMA8653 accelerometer
 */

#ifndef ACCEL_CONSTANTS_H_
#define ACCEL_CONSTANTS_H_

/**
 * I2C address
 */
#define ACCEL_I2C_ADDR (0x3au)

/**
 * 8 MSBs of 10-bit X-axis sample
 */
#define ACCEL_OUT_X_MSB                 (0x01u)

/**
 * 2 LSBs of 10-bit X-axis sample
 */
#define ACCEL_OUT_X_LSB                 (0x02u)

/**
 * 8 MSBs of 10-bit Y-axis sample
 */
#define ACCEL_OUT_Y_MSB                 (0x03u)

/**
 * 2 LSBs of 10-bit Y-axis sample
 */
#define ACCEL_OUT_Y_LSB                 (0x04u)

/**
 * 8 MSBs of 10-bit Z-axis sample
 */
#define ACCEL_OUT_Z_MSB                 (0x05u)

/**
 * 2 LSBs of 10-bit Z-axis sample
 */
#define ACCEL_OUT_Z_LSB                 (0x06u)

/**
 * Freefall/Motion functional block configuration
 *
 * x___ ____ Event latch enable: 0 = reading FF_MT_SRC clears the event flag
 * _x__ ____ Motion detect/Freefall detect: 0 = Freefall flag (logical AND combination)
 * __x_ ____ Event flag enable on Z
 * ___x ____ Event flag enable on Y
 * ____ x___ Event flag enable on X
 * ____ _xxx Unused
 */
#define ACCEL_REG_FF_MT_CFG             (0x15u)

/**
 * Freefall/Motion threshold register
 *
 * x___ ____ Debounce counter mode: 0 = Increments of decrements debounce, 1 = increments or clears
 * -xxx xxxx Threashold
 */
#define ACCEL_REG_FF_MT_THS             (0x17u)

/**
 * Data rates, ACTIVE mode
 *
 * xx__ ____ Configures auto-wake sample frequency (see table 56)
 * __xx x___ Data rate selection (see table 57)
 * ____ _x__ Unused
 * ____ __x_ Fast-read mode: 0 = normal mode
 * ____ ___x Full-scale selection: 0 = Standby, 1 = Active
 */
#define ACCEL_REG_CTRL_1                (0x2au)

/**
 * Wake from sleep, IPOL, PP_OD
 *
 * xx__ ____ Unused
 * __x_ ____ Wake from orientation interrupt
 * ___x ____ Unused
 * ____ x___ Wake from freefall/motion interrupt
 * ____ _x__ Unused
 * ____ __x_ Interrupt polarity: 0 = active low
 * ____ ___x Push-pull/open-drain: 0 = push-pull
 */
#define ACCEL_REG_CTRL_3                (0x2cu)

/**
 * Interrupt enable register (0 = disabled, 1 = enabled)
 *
 * x___ ____ Auto-sleep/wake interrupt
 * _xx_ ____ Unused
 * ___x ____ Orientation interrupt
 * ____ x___ Unused
 * ____ _x__ Freefall/motion interrupt
 * ____ __x_ Unused
 * ____ ___x Data ready interrupt
 */
#define ACCEL_REG_INT_EN                (0x2du)

/**
 * Interrupt pin (INT1/INT2) map (0 = routed to INT2 pin, 1 = routed to INT1 pin)
 *
 * x___ ____ Auto-sleep/wake interrupt
 * _xx_ ____ Unused
 * ___x ____ Orientation interrupt
 * ____ x___ Unused
 * ____ _x__ Freefall/motion interrupt
 * ____ __x_ Unused
 * ____ ___x Data ready interrupt
 */
#define ACCEL_REG_INT_PIN_MAP           (0x2eu)

#endif // ACCEL_CONSTANTS_H_
