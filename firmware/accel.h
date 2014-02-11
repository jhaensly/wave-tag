/**
 * @file accel.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
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
 * A callback to handle an accelerometer event. Only the most recently set callback will be
 * executed for a given event.
 *
 * @warning This will be called from an interrupt context, so design
 * accoridngly.
 */
typedef void (*accel_cb_t)(void);


/**
 * Configure and enable the accelerometer's freefall detect mode.
 *
 * @return error_t
 */
extern error_t accelEnableFreefall(accel_cb_t freefall_event_cb);


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
