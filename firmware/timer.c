/**
 * @file timer.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "timer.h"
#include "module_id.h"

static bool     m_timer0_recurring;
static bool     m_timer1_recurring;

static handle_timer_expired_t m_timer0_cb;
static handle_timer_expired_t m_timer1_cb;

error_t timerInit(void) {
    // The reset state has the timers enabled, so there should be no need to
    // enable them here. If we ever implement a software reset that does
    // not reinitialize the peripheral registers, uncomment the following line.
    // PRR &= (uint8_t)~((1 << PRTIM0) | (1 << PRTIM1));

    // Configure both timers to:
    //   Count up
    //   Stop at the value in the output compare register
    //   Reset back to zero
    //   Use a clock divider of 1

    TCCR0A = (1 << CTC0);
    // TCCR0B = 0; // Default
    // TIMSK0 = 0; // Default

    TCCR1A = (1 << COM1A1);
    // TCCR1B = 0; // Default
    // TIMSK1 = 0; // Default

    // Neither timers are in use. Shut them down to save power.
    PRR |= (1 << PRTIM0) | (1 << PRTIM1);

    return ERR_NONE;
}

error_t timer0Start(handle_timer_expired_t cb,
                    uint8_t ticks,
                    bool recurring) {
    if (!cb || (ticks == 0)) {
        return ERR_TIMER_INVALID_PARAM;
    }
    if (!(PRR & (1 << PRTIM0))) {
        return ERR_TIMER_BUSY;
    }

    m_timer0_cb         = cb;
    m_timer0_recurring  = recurring;

    // Enable power to the timer 0 module
    PRR   &= ~(1 << PRTIM0);

    // Count up to the appropriate value
    OCR0A  = ticks;
    TCNT0  = 0;

    // Enable the timer interrupt
    TIMSK0 = (1 << OCIE0A);

    // Start the clock
    TCCR0A |= (1 << CS00);

    return ERR_NONE;
}

error_t timer0Stop(void) {
    if (PRR & (1 << PRTIM0)) {
        return ERR_TIMER_STOPPED;
    }
    // Stop the clock
    TCCR0A &= ~0x07;

    // Disable the interrupt
    TIMSK0 = 0;

    // Cut power to the timer
    PRR   |= (1 << PRTIM0);

    return ERR_NONE;
}

error_t timer1Start(handle_timer_expired_t cb,
                    uint16_t ticks,
                    bool recurring) {
    if (!cb || (ticks == 0)) {
        return ERR_TIMER_INVALID_PARAM;
    }
    if (!(PRR & (1 << PRTIM1))) {
        return ERR_TIMER_BUSY;
    }

    m_timer1_cb         = cb;
    m_timer1_recurring  = recurring;

    // Enable power to the timer 1 module
    PRR   &= ~(1 << PRTIM1);

    // This is a 16-bit register that requires careful access. Fortunately, the
    // compiler should handle that for us.
    OCR1A  = ticks;

    // Enable the timer interrupt
    TIMSK1 = (1 << OCIE1A);

    // Start the clock
    TCCR1B |= (1 << CS10);

    return ERR_NONE;
}

error_t timer1Stop(void) {
    if (PRR & (1 << PRTIM1)) {
        return ERR_TIMER_STOPPED;
    }

    // Stop the clock
    TCCR1B &= ~0x07;

    // Disable the interrupt
    TIMSK1 = 0;

    // Cut power to the timer
    PRR   |= (1 << PRTIM1);

    return ERR_NONE;
}

ISR(TIMER0_COMPA_vect) {
    // For speed, don't check for NULL
    m_timer0_cb();

    if (!m_timer0_recurring) {
        TCCR0A &= ~0x07;
        TIMSK0  = 0;
        PRR    |= (1 << PRTIM0);
    }
}

ISR(TIMER1_COMPA_vect) {
    // For speed, don't check for NULL
    m_timer1_cb();

    if (!m_timer1_recurring) {
        TCCR1B &= ~0x07;
        TIMSK1  = 0;
        PRR    |= (1 << PRTIM1);
    }
}
