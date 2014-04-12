/**
 * @file accel.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * Driver for the Freescale MMA8653FC 3 axis accelerometer.
 */

#ifndef ACCEL_H_
#define ACCEL_H_

#include <stdint.h>
#include "module_id.h"

/**
 * Define the accelerometer axes
 */
enum accel_axis_t {
    ACCEL_X,    ///< X axis
    ACCEL_Y,    ///< Y axis
    ACCEL_Z     ///< Z axis
};


/**
 * Configure and enable the accelerometer's freefall detect mode.
 *
 * @param[in] event_cb A callback to execute when a freefall event is detected.
 *
 * @return enum error_t
 */
extern enum error_t accelEnableFreefall(void (* event_cb)(void));


/**
 * Put the accelerometer in its lowest power mode.
 *
 * @return enum error_t
 */
extern enum error_t accelDisable(void);


/**
 * Read the value of the specified axis.
 *
 * @param[in]   axis    The axis to read.
 * @param[out]  data    The data read.
 * @return enum error_t
 */
extern enum error_t accelReadValue(enum accel_axis_t axis, int8_t *data);

#endif // ACCEL_H_
