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

static bool sleepAtomicCheck(bool (* sleep_while)(void))
{
    cli();
    bool ret = sleep_while();
    sei();

    return ret;
}

void sleep(enum sleep_mode_t mode,
           bool (* sleep_while)(void))
{
    /*
     * The following code was shamelessly "inspired" by the example in
     * avr/sleep.h, demonstrating an atomic test of "condition" with interrupts
     * disabled, then entering sleep mode if the condition is true.
     */
    if (sleep_while) {
        /** @todo Choose sleep mode based on peripheral clock state */
        set_sleep_mode(mode);
        while (sleepAtomicCheck(sleep_while)) {
            sleep_enable();
            sleep_cpu();
            sleep_disable();
        }
    }
}
