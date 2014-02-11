/**
 * @file accel_constants.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 21 Jan 2014
 *
 * Helpful constants for accessing the Freescale MMA8653FC accelerometer
 */

#ifndef ACCEL_CONSTANTS_H_
#define ACCEL_CONSTANTS_H_

enum {
    ACCEL_I2C_ADDR  = 0x3a, ///< I2C address
    ACCEL_OUT_X_MSB = 0x01, ///< 8 MSBs of 10-bit X-axis sample
    ACCEL_OUT_X_LSB = 0x02, ///< 2 LSBs of 10-bit X-axis sample
    ACCEL_OUT_Y_MSB = 0x03, ///< 8 MSBs of 10-bit Y-axis sample
    ACCEL_OUT_Y_LSB = 0x04, ///< 2 LSBs of 10-bit Y-axis sample
    ACCEL_OUT_Z_MSB = 0x05, ///< 8 MSBs of 10-bit Z-axis sample
    ACCEL_OUT_Z_LSB = 0x06, ///< 2 LSBs of 10-bit Z-axis sample

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
    ACCEL_REG_FF_MT_CFG = 0x15u,


    /**
     * Freefall/Motion threshold register
     *
     * x___ ____ Debounce counter mode: 0 = Increments or decrements debounce, 1 = increments or clears
     * -xxx xxxx Threshold
     */
    ACCEL_REG_FF_MT_THS = 0x17,


    /**
     * Data rates, ACTIVE mode
     *
     * xx__ ____ Configures auto-wake sample frequency (see table 56)
     * __xx x___ Data rate selection (see table 57)
     * ____ _x__ Unused
     * ____ __x_ Fast-read mode: 0 = normal mode
     * ____ ___x Full-scale selection: 0 = Standby, 1 = Active
     */
    ACCEL_REG_CTRL_1 = 0x2a,


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
    ACCEL_REG_CTRL_3 = 0x2c,


	/**
     * Freefall debounce count.
     *
     * Actual time depends on ODR setting see table 49 on datasheet
     */
	ACCEL_REG_FF_MT_COUNT = 0x18,


    /**
     * System mode register
     *
     * xxxx xx__ Unused
     * ____ __xx System Mode
     */
    ACCEL_SYSMOD         = 0x0B,
    ACCEL_SYSMOD_STANDBY = 0x00,
    ACCEL_SYSMOD_WAKE    = 0x01,
    ACCEL_SYSMOD_SLEEP   = 0x02,

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
    ACCEL_REG_CTRL_4 = 0x2d,


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
    ACCEL_REG_INT_PIN_MAP = 0x2e,
};

#endif // ACCEL_CONSTANTS_H_
