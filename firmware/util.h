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

#define BUSY_WHILE(c) while(c)
#define BUSY_UNTIL(c) BUSY_WHILE(!(c))

#define FIND_MIDPOINT(min, max) ((min) + (((max) - (min)) / 2))

#define OUTPUT_VALUE(v) do { PORTB = (uint8_t)~((v << 6) | (v >> 2)); } while(0)

#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

#endif // UTIL_H_
