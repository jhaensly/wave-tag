/**
 * @file adc.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 12 Feb 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "module_id.h"

/**
 * ADC channels
 */
enum adc_channel_t {
    ADC_CHAN_0      = 0x00, ///< Channel 0
    ADC_CHAN_1      = 0x01, ///< Channel 1
    ADC_CHAN_2      = 0x02, ///< Channel 2
    ADC_CHAN_3      = 0x03, ///< Channel 3
    ADC_CHAN_4      = 0x04, ///< Channel 4
    ADC_CHAN_5      = 0x05, ///< Channel 5
    ADC_CHAN_6      = 0x06, ///< Channel 6
    ADC_CHAN_7      = 0x07, ///< Channel 7
    ADC_CHAN_8      = 0x08, ///< Channel 8; reads the on-chip temperature sensor
    ADC_CHAN_1_1V   = 0x0e, ///< Internal 1.1V reference
    ADC_CHAN_GND    = 0x0f, ///< Ground reference

    ADC_CHAN_MASK   = 0x0f, ///< Bitmask for valid channel values
};


/**
 * Initialize the ADC module. This should be called prior to any other
 * operations.
 *
 * @return
 */
extern enum error_t adcInit(void);


/**
 * Take an ADC measurement, returning the measurement as an argument to the
 * supplied callback. The caller can continue to work while the measurement is
 * in progress, or sleep in ADC noise reduction mode (or higher).
 *
 * @param[in] channel The ADC channel to sample.
 * @param[in] adc_cb The callback to execute when the measurement is complete.
 *
 * @return
 */
extern enum error_t adcIntEnable(enum adc_channel_t channel,
                                 void (* adc_cb)(uint8_t));


/**
 *
 */
extern enum error_t adcEnable(enum adc_channel_t channel);


/**
 * Disable the ADC peripheral.
 *
 * @return
 */
extern enum error_t adcDisable(void);

#endif // ADC_H_
