/**
 * @file adc.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 12 Feb 2014
 *
 * @copyright Copyright details go here
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "module_id.h"

typedef enum {
    ADC_CHAN_0      = 0x00,
    ADC_CHAN_1      = 0x01,
    ADC_CHAN_2      = 0x02,
    ADC_CHAN_3      = 0x03,
    ADC_CHAN_4      = 0x04,
    ADC_CHAN_5      = 0x05,
    ADC_CHAN_6      = 0x06,
    ADC_CHAN_7      = 0x07,
    ADC_CHAN_8      = 0x08,
    ADC_CHAN_1_1V   = 0x0e,
    ADC_CHAN_GND    = 0x0f,

    ADC_CHAN_MASK   = 0x0f,
} adc_channel_t;


typedef void (*adc_cb_t)(uint8_t);

extern error_t adcInit(void);

extern error_t adcIntEnable(adc_channel_t channel, adc_cb_t cb);

extern error_t adcEnable(adc_channel_t channel);

extern error_t adcDisable(void);

#endif // ADC_H_
