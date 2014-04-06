/**
 * @file button.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 09 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "button.h"

#define GET_BUTTON_STATE() ((PIND & _BV(PIND2)) ? BUTTON_RELEASED : BUTTON_PRESSED)

static volatile button_state_t m_button_state;
static button_cb_t m_cb;

error_t buttonEnable(void) {
    // PORTD = 0; // Default

    // Init internal variables
    m_button_state = GET_BUTTON_STATE();

    // Button interrupt on any edge
    EICRA |= _BV(ISC00);
    EIMSK |= _BV(INT0);

    return ERR_NONE;
}


error_t buttonDisable(void) {
    EIMSK &= ~_BV(INT0);
    m_button_state = BUTTON_DISABLED;
    return ERR_NONE;
}


button_state_t buttonState(void) {
    return m_button_state;
}


ISR (INT0_vect) {
    m_button_state = GET_BUTTON_STATE();

    if (m_cb && (m_button_state == BUTTON_RELEASED)) {
        m_cb(BUTTON_SINGLE_PRESS);
    }
}
