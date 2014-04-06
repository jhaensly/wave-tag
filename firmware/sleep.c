/**
 * @file sleep.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 07 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/interrupt.h>
#include "sleep.h"

static bool atomicCheck(sleep_while_condition_t f_cond) {
    bool ret;

    cli();
    ret = f_cond();
    sei();

    return ret;
}

void sleep(sleep_mode_t mode, sleep_while_condition_t f_cond) {
    // The following code was shamelessly "inspired" by the example in
    // avr/sleep.h, demonstrating an atomic test of "condition" with interrupts
    // disabled, then entering sleep mode if the condition is true.

    if (f_cond) {
        set_sleep_mode(mode);
        while (atomicCheck(f_cond)) {
            sleep_enable();
            sleep_cpu();
            sleep_disable();
        }
    }
}
