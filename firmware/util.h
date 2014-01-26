/**
 * @file util.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>

#define BUSY_WHILE(c) while(c)
#define BUSY_UNTIL(c) BUSY_WHILE(!(c))

#define FIND_MIDPOINT(min, max) ((min) + (((max) - (min)) / 2))

#define OUTPUT_VALUE(v) do { PORTB = ~((uint8_t)(v << 6) | (uint8_t)(v >> 2)); } while(0)

#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

#endif // UTIL_H_
