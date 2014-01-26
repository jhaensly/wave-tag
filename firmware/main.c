/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "util.h"
#include "accel.h"

// Edge detection sensitivity
#define LED_MEASUREMENT_SENSITIVITY (5)


volatile bool    is_button_down = false;
volatile uint8_t led_measurement_min;
volatile uint8_t led_measurement_max;
volatile uint8_t led_measurement_thresh;
volatile uint8_t led_measurement_time;
volatile uint8_t led_measurement_bit;

volatile uint8_t vlc_data[64];
volatile uint8_t vlc_data_idx = 0;

enum {
    MODE_ERROR,
    MODE_SLEEP,
    MODE_WAITING,
    MODE_VLC,
    MODE_WAVE,
    MODE_ACCEL_TEST,
} app_mode;

void error_state(uint8_t err_code) {
    bool is_on = false;
    int i = 20;

    while (i-- > 0) {
        OUTPUT_VALUE(is_on ? err_code : 0);
        is_on = !is_on;
        _delay_ms(10);
    }

    // TODO: Reset device
}



uint8_t measureLED() {
    PORTC = PORTC & 0xfcu; //kill both sides of the LED
    DDRC = 0x03u;

    //configure ADC
    ADMUX  = 0x61u; //left adjust, avcc ref, ADC1
    ADCSRA = 0x80u; //enable ADC, divide clock by 2

    //raise the cathode
    PORTC |= 0x01u;
    _delay_us(100);

    //take the LED anode out of the equation.
    DDRC = 0x01u;
    _delay_us(10);

    //START CONVERSION
    ADCSRA|=0b01000000;

    while (ADCSRA & 0b01000000) {
        ADCSRA |= (0b00010000);
    }

    uint8_t temp = ADCH;

    ADCSRA = 0x00u;  //disable ADC
    PORTC &= 0xf8u;
    DDRC   = 0x03u;  //return to normal
    ADMUX  = 0x00u;
    ADCSRA = 0x00u;
    return temp;
}


int main(void) {
    DDRB  = 0xffu;    //LED PINS
    DDRD  = 0x00u;
    PORTD = 0x00u;

    OUTPUT_VALUE(5u);

    //Timer0 interrupt
	//OCR0A = 50; //how high you count
	//TCCR0A = 0x0A; //compare match CTC, no multiplier
	//TIMSK0 = 0x02; //enable compare match interrupt.
	//TCNT0 = 0;

    //configure interrupt
    EICRA = 0x0cu; //rising edge
    EIMSK = 0x02u;
    sei();

    OUTPUT_VALUE(0);

    accelConfigFreefall();
    app_mode = MODE_ACCEL_TEST;

    while(1) {
        switch (app_mode)
        {
        case MODE_SLEEP:
            // Go to sleep
            break;

        case MODE_WAITING:
            // User input
            break;

        case MODE_VLC:
            // Accept user input
            break;

        case MODE_WAVE:
            // Wave!!
            break;

        case MODE_ACCEL_TEST:
        {
            accel_data_t val;
            accelReadValue(ACCEL_Y, &val);
            OUTPUT_VALUE(val);
            _delay_ms(10);
            break;
        }

        case MODE_ERROR:
            // Fall through
        default:
            error_state(0xffu);
            break;
        }
    }
}

ISR (INT1_vect) {
   // _delay_ms(1);
   // OUTPUT_VALUE(0x00);
}


ISR(TIMER0_COMPA_vect) {
    if (PIND & 4) {
        // Button released
        is_button_down = false;
    }
    else {
        // Button pressed
        uint8_t led_measurement = measureLED();

        if (!is_button_down) {
            // Start the VLC measurement state machine. Reset measurement thresholds
            is_button_down = true;
            led_measurement_max     = led_measurement;
            led_measurement_min     = led_measurement;
            led_measurement_thresh  = led_measurement + (led_measurement/2);
            led_measurement_time    = 0;
            led_measurement_bit     = 0;
            return;
        }

        // Maximum and minimum values are primarily a function of the transmitter's distance from
        // the board. Since that is variable, find good values dynamically. At the start of a
        // transmission, they should change frequenctly. Later, as things stabilize, we should
        // jump to .
        if (led_measurement > led_measurement_max) {
            // Reset max threshold
            led_measurement_max = led_measurement;
            led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
        }
        else if (led_measurement < led_measurement_min)
        {
            // Reset min threshold
            led_measurement_min = led_measurement;
            led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
        }
        // At this point, we have established the dynamic range and set our threshold at the midpoint.
        // The VLC protocol encodes information on the duration between edges, so look for edges.
        else if ((led_measurement_bit == 0) &&
                 (led_measurement > (led_measurement_thresh + LED_MEASUREMENT_SENSITIVITY))) {
            // Handle rising edge
            led_measurement_bit = 1;
            OUTPUT_VALUE(led_measurement_time);
            led_measurement_time = 0;
        }

        else if((led_measurement_bit == 1) &&
                (led_measurement < (led_measurement_thresh - LED_MEASUREMENT_SENSITIVITY))) {
            // Handle falling edge
            led_measurement_bit = 0;
            OUTPUT_VALUE(led_measurement_time);
            led_measurement_time = 0;
        }
        // If no edge has been detected, increment the time. By doing this in the else, it saves us
        // from a particularly noisy initial measurement.
        else {
            led_measurement_time++;
        }
    }
}

