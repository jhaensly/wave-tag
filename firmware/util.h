/**
 * @file util.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * A handful of function/macros that are widely useful.
 */

#ifndef UTIL_H_
#define UTIL_H_

/// Block execution while the given condition is true
#define BUSY_WHILE(c) while(c)

/// Block execution until the given condition is false
#define BUSY_UNTIL(c) BUSY_WHILE(!(c))

/// Find the midpoint of two values, rounding down
#define FIND_MIDPOINT(min, max) ((min) + (((max) - (min)) / 2))

/// Output the following unsigned byte to the LED array
#define OUTPUT_VALUE(v) do { PORTB = (uint8_t)~((uint8_t)(v << 6) | (uint8_t)(v >> 2)); } while(0)

/// Get the number of elements in an array at compile time
#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

#endif // UTIL_H_
