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
typedef uint8_t accel_data_t;

/**
 * Configure and enable the accelerometer's freefall detect mode.
 *
 * @return 0 if success, nonzero otherwise.
 */
extern int accelConfigFreefall();

/**
 * Read the value of the specified axis.
 *
 * @param[in]   axis    The axis to read.
 * @param[out]  data    The data read.
 * @return 0 if success, nonzero otherwise.
 */
extern int accelReadValue(accel_axis_t axis, accel_data_t* data);

#endif // ACCEL_H_
