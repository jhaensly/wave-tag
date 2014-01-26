/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "util.h"
#include "accel.h"
#include "alphabet.h"


// Edge detection sensitivity
#define LED_MEASUREMENT_SENSITIVITY (5)


static volatile bool m_vlc_in_progress;
volatile uint8_t led_measurement_min;
volatile uint8_t led_measurement_max;
volatile uint8_t led_measurement_thresh;
volatile uint8_t led_measurement_time;
volatile uint8_t led_measurement_bit;

volatile uint8_t vlc_data[64];
volatile uint8_t vlc_data_idx = 0;


enum {
    MODE_SLEEP,
    MODE_WAITING,
    MODE_VLC,
    MODE_WAVE,
    MODE_ACCEL_TEST,
    MODE_COUNT_TEST,
} m_current_mode,
  m_next_mode;


void error_state(int err_code) {
    int i = 20;

    while (i-- > 0) {
        OUTPUT_VALUE(err_code);
        _delay_ms(100);

        OUTPUT_VALUE(0);
        _delay_ms(100);
    }

    // TODO: Reset device
}

#if 0
static void handleButtonEvent(button_event_t event) {
    switch (event) {
        case BUTTON_SINGLE_PRESS:
            break;

        case BUTTON_DOUBLE_PRESS:
            break;

        case BUTTON_HOLD_START:
            m_vlc_in_progress       = true;
            led_measurement_max     = 0;
            led_measurement_min     = UINT8_MAX;
            led_measurement_thresh  = UINT8_MAX / 2;
            led_measurement_time    = 0;
            led_measurement_bit     = 0;

            // start timer
            break;

        case BUTTON_HOLD_RELEASE:
            m_vlc_in_progress = false;
            // stop timer
            break;

        default:
            break;
    }
}
#endif

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


static int doSleep(void) {
    return -1;
}

static int doWaiting(void) {
    return -2;
}

static int doVLC(void) {
    return -3;
}

static int doWave(void) {
    initDisplay();
    outputText[0][0]=1;
    outputText[0][1]=2;
    outputText[0][2]=3;
    refreshFrameBuffer();
    while(1);
    return 0;
}

static int doAccelTest(void) {
    while (m_current_mode == m_next_mode) {
        accel_data_t val;
        accelReadValue(ACCEL_Y, &val);
        OUTPUT_VALUE(val);
        _delay_ms(10);
    }

    return 0;
}

static int doCountTest(void) {
    uint8_t i;
    while(1)
    {
        i++;
        OUTPUT_VALUE(i);
        _delay_ms(10);
    }
    return 0;
}

int main(void) {
    DDRB  = 0xffu;    //LED PINS
    DDRD  = 0x00u;
    PORTD = 0x00u;


    //Timer0 interrupt
	//OCR0A = 50; //how high you count
	//TCCR0A = 0x0A; //compare match CTC, no multiplier
	//TIMSK0 = 0x02; //enable compare match interrupt.
	//TCNT0 = 0;

    //configure interrupt


    //buttonInit();
    //buttonRegisterEventHandler(&handleButtonEvent);
    accelConfigFreefall();
    m_current_mode = MODE_COUNT_TEST;
    m_next_mode = MODE_COUNT_TEST;
    doCountTest();
    while(1) {
        int error;

        switch (m_current_mode)
        {
        case MODE_SLEEP:
            error = doSleep();
            break;

        case MODE_WAITING:
            error = doWaiting();
            break;

        case MODE_VLC:
            error = doVLC();
            break;

        case MODE_WAVE:
            error = doWave();
            break;

        case MODE_ACCEL_TEST:
            error = doAccelTest();
            break;

        case MODE_COUNT_TEST:
            error = doCountTest();
            break;
                
        default:
            error = -1;
            break;
        }

        if (error != 0) {
            error_state(error);
        }

        m_current_mode = m_next_mode;
    }
}



/**
 * Interrupt handler for timer.  In charge of displaying text.
 */
/*ISR(TIMER0_COMPA_vect) {
    timerZeroHandler();
       
}*/


/**
 * Interrupt handler for accelerometer interrupt
 */

/*ISR (INT1_vect)
{
    intOneHandler();
}*/




/*
ISR (INT1_vect) {
   // _delay_ms(1);
   // OUTPUT_VALUE(0x00);
}*/

/*
ISR(TIMER0_COMPA_vect) {
    if (m_vlc_in_progress) {
        uint8_t led_measurement = measureLED();

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
}*/

