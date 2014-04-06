/**
 * @file display.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 07 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/io.h>
#include "display.h"

error_t displayEnable(void) {
    PORTB = 0xff;
    DDRB  = 0xff;
    return ERR_NONE;
}

error_t displayDisable(void) {
    // Configure as input
    PORTB = 0;
    DDRB  = 0;
    return ERR_NONE;
}

void displayByte(uint8_t v) {
    PORTB = (uint8_t)~((v << 6) | (v >> 2));
}

void displayGlyph(uint8_t g) {
    PORTB = g;
}
