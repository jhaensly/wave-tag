/**
 * @file timer.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "timer.h"
#include "module_id.h"

static bool     m_timer0_recurring;
//static bool     m_timer1_recurring;

static timer_cb_t m_timer0_cb;
//static timer_cb_t m_timer1_cb;

error_t timerInit(void) {
    // The reset state has the timers enabled, so there should be no need to
    // enable them here. If we ever implement a software reset that does
    // not reinitialize the peripheral registers, uncomment the following line.
    // PRR &= (uint8_t)~(_BV(PRTIM0) | _BV(PRTIM1));

    // Configure both timers to:
    //   Count up
    //   Stop at the value in the output compare register
    //   Reset back to zero

    TCCR0A = _BV(CTC0);
    // TIMSK0 = 0; // Default

#if 0 // For some reason, the following two lines cause LED 4 to stay on
    TCCR1A = _BV(COM1A1);
    TCCR1B = _BV(CS12); // Tick = F_CPU / 1024, or approx. 1 msec
    // TIMSK1 = 0;      // Default
#endif

    // Neither timer's in use. Shut them down to save power.
    PRR |= _BV(PRTIM0) | _BV(PRTIM1);

    return ERR_NONE;
}

error_t timer0Start(timer_cb_t cb,
                    uint8_t usec,
                    bool recurring) {
    if (!cb || (usec == 0)) {
        return ERR_TIMER_INVALID_PARAM;
    }
    if (!(PRR & _BV(PRTIM0))) {
        return ERR_TIMER_BUSY;
    }

    m_timer0_cb         = cb;
    m_timer0_recurring  = recurring;

    // Enable power to the timer 0 module
    PRR   &= ~_BV(PRTIM0);

    // Reset the counter to 0
    TCNT0  = 0;

    // Count up to the appropriate value
    OCR0A  = usec;

    // Enable the timer interrupt
    TIMSK0 = _BV(OCIE0A);

    // Start the clock
    TCCR0A |= _BV(CS00);

    return ERR_NONE;
}

error_t timer0Stop(void) {
    if (PRR & _BV(PRTIM0)) {
        return ERR_TIMER_STOPPED;
    }
    // Stop the clock
    TCCR0A &= ~0x07;

    // Disable the interrupt
    TIMSK0 = 0;

    // Cut power to the timer
    PRR   |= _BV(PRTIM0);

    return ERR_NONE;
}

error_t timer0Restart(timer_cb_t cb) {
    if (PRR & _BV(PRTIM0)) {
        return ERR_TIMER_STOPPED;
    }
    if (!cb) {
        return ERR_TIMER_INVALID_PARAM;
    }

    // Disable the interrupt to guard against possible race conditions.
    TIMSK0 = 0;

    // Reset the callback
    m_timer0_cb = cb;

    // CPU writes to the counter register take precidence over all other
    // accesses, so this should be safe to do without stopping the clock.
    TCNT0 = 0;

    // Clear any interrupt flags that may have been raised.
    TIFR0 |= _BV(OCF0A);

    // Reenable the interrupt.
    TIMSK0 = _BV(OCIE0A);

    return ERR_NONE;
}

#if 0
error_t timer1Start(timer_cb_t cb,
                    uint16_t msec,
                    bool recurring) {
    if (!cb || (msec == 0)) {
        return ERR_TIMER_INVALID_PARAM;
    }
    if (!(PRR & _BV(PRTIM1))) {
        return ERR_TIMER_BUSY;
    }

    m_timer1_cb         = cb;
    m_timer1_recurring  = recurring;

    // Enable power to the timer 1 module
    PRR   &= ~_BV(PRTIM1);

    // These are 16-bit registers that requires careful access. Fortunately, the
    // compiler should handle that for us.
    OCR1A  = msec;
    TCNT1  = 0;

    // Enable the timer interrupt
    TIMSK1 = _BV(OCIE1A);

    // Start the clock
    TCCR1B |= _BV(CS10);

    return ERR_NONE;
}

error_t timer1Stop(void) {
    if (PRR & _BV(PRTIM1)) {
        return ERR_TIMER_STOPPED;
    }

    // Stop the clock
    TCCR1B &= ~0x07;

    // Disable the interrupt
    TIMSK1 = 0;

    // Cut power to the timer
    PRR   |= _BV(PRTIM1);

    return ERR_NONE;
}

error_t timer1Restart(void) {
    if (PRR & _BV(PRTIM1)) {
        return ERR_TIMER_STOPPED;
    }

    // Disable the interrupt.
    TIMSK1 = 0;

    // CPU writes to the counter register take precidence over all other
    // accesses, so this should be safe to do without stopping the clock.
    TCNT1 = 0;

    // Reenable the interrupt.
    TIMSK1 = _BV(OCIE1A);

    return ERR_NONE;
}
#endif


ISR(TIMER0_COMPA_vect) {
    // For speed, don't check for NULL
    m_timer0_cb();

    if (!m_timer0_recurring) {
        TCCR0A &= ~0x07;
        TIMSK0  = 0;
        PRR    |= _BV(PRTIM0);
    }
}

#if 0
ISR(TIMER1_COMPA_vect) {
    // For speed, don't check for NULL
    m_timer1_cb();

    if (!m_timer1_recurring) {
        TCCR1B &= ~0x07;
        TIMSK1  = 0;
        PRR    |= _BV(PRTIM1);
    }
}
#endif
