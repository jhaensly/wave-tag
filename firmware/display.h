/**
 * @file display.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 27 Jan 2014
 *
 * The Wavetag display consists of 8 LEDs driven directly by the
 * microcontroller.
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

/// Output an unsigned byte to the display
#define OUTPUT_VALUE(v) \
    do { PORTB = (uint8_t)~((uint8_t)(v << 6) | (uint8_t)(v >> 2)); } while(0)

#endif // DISPLAY_H_
