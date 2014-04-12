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

/* Private variables */
static void (* m_cb)(uint8_t);


/* Function definitions */
enum error_t adcInit(void)
{
    /* Enable power to the ADC peripheral before making changes */
    PRR &= ~_BV(PRADC); // Default

    /* Left adjust result and use AVcc for reference */
    ADMUX = _BV(ADLAR) | _BV(REFS0);

    /* Cut power to the ADC peripheral while not in use */
    PRR |= _BV(PRADC);

    return ERR_NONE;
}

enum error_t adcEnable(enum adc_channel_t channel)
{
    /* Enable the peripheral clock */
    PRR &= ~_BV(PRADC);

    /* Left adjust; Use VCC as reference */
	ADMUX  = _BV(REFS0) | _BV(ADLAR) | channel;

    /* Use clock divider of 2; enable ADC */
	ADCSRA = _BV(ADEN);

    return ERR_NONE;
}

enum error_t adcIntEnable(enum adc_channel_t channel,
                          void (* adc_cb)(uint8_t))
{
    if (!adc_cb) {
        return ERR_ADC_INVALID_ARG;
    } else if (!(PRR & _BV(PRADC))) {
        return ERR_ADC_BUSY;
    }

    m_cb = adc_cb;

    /* Enable the ADC peripheral clock */
    PRR &= ~_BV(PRADC);

	/* Select the channel */
    ADMUX |= channel;

    /*
     * Enable the ADC and start conversion. The first measurement will take 25
     * clock cycles rather than the typical 13.
     */
	ADCSRA |= _BV(ADEN) | _BV(ADSC) | _BV(ADIE);

    /* Disable the peripheral clock */
    PRR    |= _BV(PRADC);

    return ERR_NONE;
}

enum error_t adcDisable()
{
    if (PRR & _BV(PRADC)) {
        return ERR_ADC_STOPPED;
    }

	ADCSRA &= ~(_BV(ADEN) | _BV(ADSC));
    ADMUX  &= ~ADC_CHAN_MASK;
    PRR    |= _BV(PRADC);

    return ERR_NONE;
}

ISR (ADC_vect)
{
    static uint8_t val;

    val = ADCH;
    m_cb(val);
}
