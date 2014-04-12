/**
 * @file display.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 07 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * Display controls
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "module_id.h"

/**
 * Initialize the display.
 *
 * @return
 */
extern enum error_t displayEnable(void);


/**
 * Put the display into its lowest power configuration.
 *
 * @todo Configure as inputs
 */
extern enum error_t displayDisable(void);


/**
 * Display an unsigned byte using binary notation.
 */
extern void displayByte(uint8_t v);


/**
 * Display a glyph that has been optimized for this display layout.
 */
extern void displayGlyph(uint8_t g);

#endif // DISPLAY_H_
