/**
 * @file adc.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 12 Feb 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

static adc_cb_t m_cb;

error_t adcInit(void) {
    // Enable power to the ADC peripheral before making changes
    PRR &= ~_BV(PRADC); // Default

    // Left adjust result and use AVcc for reference
    ADMUX = _BV(ADLAR) | _BV(REFS0);

    // Cut power to the ADC peripheral while not in use
    PRR |= _BV(PRADC);

    return ERR_NONE;
}

error_t adcEnable(adc_channel_t channel)
{
    //configure ADC
    PRR &= ~_BV(PRADC);
	ADMUX  = 0x60u | channel; //left adjust, avcc ref
	ADCSRA = 0x80u; //enable ADC, divide clock by 2
    return ERR_NONE;
}

error_t adcIntEnable(adc_channel_t channel, adc_cb_t cb) {
    if (!cb) {
        return ERR_ADC_INVALID_ARG;
    }
    if (!(PRR & _BV(PRADC))) {
        return ERR_ADC_BUSY;
    }

    m_cb = cb;

    // Enable power to the ADC peripheral
    PRR &= ~_BV(PRADC);

	// Set the channel
    ADMUX |= channel;

    // Enable the ADC and start conversion. The first measurement will take 25
    // clock cycles rather than the typical 13.
	ADCSRA |= _BV(ADEN) | _BV(ADSC) | _BV(ADIE);

    // Disable power to the ADC peripheral
    PRR    |= _BV(PRADC);

    return ERR_NONE;
}

error_t adcDisable() {
    if (PRR & _BV(PRADC)) {
        return ERR_ADC_STOPPED;
    }

	ADCSRA &= ~(_BV(ADEN) | _BV(ADSC));
    ADMUX  &= ~ADC_CHAN_MASK;
    PRR    |= _BV(PRADC);

    return ERR_NONE;
}

ISR (ADC_vect) {
    static uint8_t val;

    val = ADCH;
    m_cb(val);
}
