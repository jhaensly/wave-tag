/**
 * @file vlc_decoder_data.h
 *
 * @author This file was procedurally generated on 26 Jan 2014
 *
 * The decoding scheme for the Huffman encoded data used for VLC. To use, advance
 * column-wise through the matrix for 0 bits and row-wise for 1 bits. When a valid
 * symbol is encountered, record that value and resume decoding from the start.
 */

#ifndef VLC_DECODER_DATA_H_
#define VLC_DECODER_DATA_H_

#include <stdint.h>
#include <avr/pgmspace.h>

/// The number of elements in the matrix
#define VLC_DECODER_DATA_ELEMENTS (100)

/// The number of columns in each row of the matrix
#define VLC_DECODER_DATA_COLUMNS  (10)

/// The value used to denote invalid symbols
#define VLC_DECODER_DATA_INVALID  (0xff)

/// The decoder matrix
extern const uint8_t VLC_DECODER_DATA[VLC_DECODER_DATA_ELEMENTS] PROGMEM;


#endif // VLC_DECODER_DATA_H_