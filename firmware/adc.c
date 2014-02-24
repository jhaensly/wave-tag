/**
 * @file adc.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 12 Feb 2014
 *
 * @copyright Copyright details go here
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

    // Configure the ADC to divide the system clock by 8. With a CPU clock rate
    // of 1MHz and each conversion taking 13 clock cycles, that works out to a
    // sample every 104 usec. Along with the setting of the ADCSRB register
    // below, setting the ADATE bit here will enable a "free run mode" in
    // which samples are taken continuously.
    //@Ciuffo removing this line to switch timing away from ADC
    //ADCSRA = _BV(ADPS1) | _BV(ADPS0) | _BV(ADATE);

    // Setting the ADCSRB register to 0 enables free run mode.
    // ADCSRB = 0; // Default

    // Cut power to the ADC peripheral while not in use
    PRR |= _BV(PRADC);

    return ERR_NONE;
}

error_t adcEnable(adc_channel_t channel, adc_cb_t cb) {
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
