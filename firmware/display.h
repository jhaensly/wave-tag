/**
 * @file display.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 07 February 2014
 *
 * Display controls
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "module_id.h"

/**
 * Initialize the display.
 */
extern error_t displayEnable(void);


/**
 * Put the display into its lowest power configuration.
 *
 * @todo Configure as inputs
 */
extern error_t displayDisable(void);


/**
 * Display an unsigned byte using binary notation.
 */
extern void displayByte(uint8_t v);


/**
 * Display a glyph that has been optimized for this display layout.
 */
extern void displayGlyph(uint8_t g);

#endif // DISPLAY_H_
