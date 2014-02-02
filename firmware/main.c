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
#include "vlc.h"


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




static int doSleep(void) {
    return -1;
}

static int doWaiting(void) {
    return 0;
}

static int doVLC(void) {
    enableVLC();
	//_delay_us(100);
	BUSY_UNTIL(PIND&0x04);
	disableVLC();
	m_next_mode = MODE_WAVE;
	return 0;
}

static int doWave(void) {
	//while button is pressed and held, stay in VLC mode
	initDisplay();
	refreshFrameBuffer();
	
	BUSY_WHILE(PIND&0x04);
	
	m_next_mode = MODE_VLC;
	killDisplay();
	
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
    uint8_t i =0;
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
	OUTPUT_VALUE(0X00u);



    //configure interrupt


    //buttonInit();
    //buttonRegisterEventHandler(&handleButtonEvent);
    accelConfigFreefall();
    m_current_mode = MODE_WAVE;
    m_next_mode = MODE_WAVE;
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
ISR(TIMER0_COMPA_vect) {
	if (m_current_mode==MODE_WAVE) {
		waveTimerZeroHandler();
	}
	if (m_current_mode==MODE_VLC) {
		vlcTimerZeroHandler();
	}
       
}


/**
 * Interrupt handler for accelerometer interrupt
 */

ISR (INT1_vect)
{
	if (m_current_mode==MODE_WAVE) {
		waveIntOneHandler();
	}
}





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

