/**
 * @file vlc_decoder_data.h
 *
 * @author This file was procedurally generated on 06 Apr 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * The decoding scheme for the Huffman encoded data used for VLC. The value at
 * an index represents either the index in the array of its left child or, if
 * the most significant bit is set, the value itself. The index of the right
 * child is the stored index value + 1. If there is no right or left child, the
 * value at that index will equal VLC_DECODER_DATA_INVALID
 */

#ifndef VLC_DECODER_DATA_H_
#define VLC_DECODER_DATA_H_

#include <stdint.h>
#include <avr/pgmspace.h>

enum {
    /// The number of elements in the matrix
    VLC_DECODER_DATA_ELEMENTS = 70,

    /// The value used to denote invalid symbols
    VLC_DECODER_DATA_INVALID  = 0xff,
};

/// The decoder matrix
extern const uint8_t VLC_DECODER_DATA[VLC_DECODER_DATA_ELEMENTS] PROGMEM;


#endif // VLC_DECODER_DATA_H_