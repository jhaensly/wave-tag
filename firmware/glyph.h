/**
 * @file glyph.h
 *
 * @author This file was procedurally generated on 06 Apr 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * Define the glyphs used to display a message.
 *
 * Using an 8-LED display, each column of a glyph is defined in byte
 * in which the LSB corresponds to the state of the LED on the bottom
 * row of the glyph, and each subsequent bit corresponds to each
 * subsequent LED. A value of 0 indicates the LED should be on, while
 * a value of 1 indicates the LED should be off.
 *
 * To determine the representation of a given glyph, find the start index
 * of its data in GLYPH_IDX, then the number of bytes of data (its width)
 * in GLYPH_WIDTH, then read the correct number of bytes from GLYPH_DATA.
 */

#ifndef GLYPH_H_
#define GLYPH_H_

#include <stdint.h>
#include <avr/pgmspace.h>

enum {
    /// The number of defined glyphs
    GLYPH_COUNT = 0x2b,
};

/// The start index of a glyph's data in GLYPH_DATA
extern const uint8_t GLYPH_IDX[] PROGMEM;

/// The number of bytes of data for the given glyph
extern const uint8_t GLYPH_WIDTH[] PROGMEM;

/// Glyph data
extern const uint8_t GLYPH_DATA[] PROGMEM;

#endif // GLYPH_H_