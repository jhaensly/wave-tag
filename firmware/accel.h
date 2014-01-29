/**
 * @file accel.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * Driver for the Freescale something or other
 */

#ifndef ACCEL_H_
#define ACCEL_H_

#include <stdint.h>
#include "module_id.h"

/**
 * Define the accelerometer axes
 */
typedef enum {
    ACCEL_X,
    ACCEL_Y,
    ACCEL_Z
} accel_axis_t;

/**
 * Define the accelerometer data format
 */
typedef int8_t accel_data_t;

/**
 * Initialize the accelerometer.
 */
extern error_t accelInit(void);

/**
 * Configure and enable the accelerometer's freefall detect mode.
 *
 * @return error_t
 */
extern error_t accelEnableFreefall(void);


/**
 * Put the accelerometer in its lowest power mode.
 *
 * @return error_t
 */
extern error_t accelDisable(void);

/**
 * Read the value of the specified axis.
 *
 * @param[in]   axis    The axis to read.
 * @param[out]  data    The data read.
 * @return error_t
 */
extern error_t accelReadValue(accel_axis_t axis, accel_data_t* data);

#endif // ACCEL_H_
