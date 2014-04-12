/**
 * @file util.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>

/**
 * Busy-wait while the given condition is true.
 *
 * @param[in] c The condition to evaluate.
 */
#define BUSY_WHILE(c) while(c)


/**
 * Busy-wait until the given condition is true.
 *
 * @param[in] c The condition to evaluate.
 */
#define BUSY_UNTIL(c) BUSY_WHILE(!(c))


/**
 * Find the midpoint of two values.
 *
 * @param[in] min The smaller of the two values.
 * @param[in] max The larger of the two values.
 *
 * @return The midpoint (rounded down).
 */
#define FIND_MIDPOINT(min, max) ((min) + (((max) - (min)) / 2))


/**
 * (DEPRECATED) Output a byte to the LED array.
 *
 * @param[in] v The value to output.
 */
#define OUTPUT_VALUE(v) do { PORTB = (uint8_t)~((v << 6) | (v >> 2)); } while(0)


/**
 * Calculate the number of elements in an array.
 *
 * @param[in] a The array to evaluate.
 *
 * @return The number of elements in the array.
 */
#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

#endif // UTIL_H_
