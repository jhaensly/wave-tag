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
#include "module_id.h"

#define BUTTON_RELEASED()   (PIND & 4)
#define BUTTON_PRESSED()    (!BUTTON_RELEASED())

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

static volatile enum {
    APP_MODE_SLEEP,
    APP_MODE_WAITING,
    APP_MODE_VLC,
    APP_MODE_WAVE,
    APP_MODE_ACCEL_TEST,
    APP_MODE_COUNT_TEST,
} m_current_mode,
  m_next_mode;

void error_state(error_t err_code) {
    static const uint8_t fw_ver = (APP_VERSION_MAJOR << 4) | APP_VERSION_MINOR;

    for (int i=0; i<10; i++) {
        OUTPUT_VALUE(err_code);
        _delay_ms(100);

        OUTPUT_VALUE(fw_ver);
        _delay_ms(100);
    }

    // TODO: Reset device
}


static error_t doSleep(void) {
    return ERR_APP_INVALID_MODE;
}

static error_t doWaiting(void) {
    return ERR_APP_INVALID_MODE;
}

static error_t doVLC(void) {
    error_t err = vlcEnable();

    if (err != ERR_NONE) {
        return err;
    }

	BUSY_UNTIL(BUTTON_RELEASED());
	err = vlcDisable();

	m_next_mode = APP_MODE_WAVE;
	return err;
}

static error_t doWave(void) {
	//while button is pressed and held, stay in VLC mode
	initDisplay();
	refreshFrameBuffer();

	BUSY_UNTIL(BUTTON_PRESSED());

	killDisplay();
	m_next_mode = APP_MODE_VLC;
	return ERR_NONE;
}

static error_t doAccelTest(void) {
    while (m_current_mode == m_next_mode) {
        accel_data_t val;
        accelReadValue(ACCEL_Y, &val);
        OUTPUT_VALUE(val);
        _delay_ms(10);
    }

    return ERR_NONE;
}

static error_t doCountTest(void) {
    uint8_t i = 0;
    while(1)
    {
        i++;
        OUTPUT_VALUE(i);
        _delay_ms(10);
    }
    return ERR_NONE;
}

int main(void) {
    DDRB  = 0xffu;    //LED PINS
    DDRD  = 0x00u;
    PORTD = 0x00u;
	OUTPUT_VALUE(0X00u);

    accelConfigFreefall();
    m_current_mode  = APP_MODE_WAVE;
    m_next_mode     = APP_MODE_WAVE;

    while(1) {
        error_t error;

        switch (m_current_mode)
        {
        case APP_MODE_SLEEP:
            error = doSleep();
            break;

        case APP_MODE_WAITING:
            error = doWaiting();
            break;

        case APP_MODE_VLC:
            error = doVLC();
            break;

        case APP_MODE_WAVE:
            error = doWave();
            break;

        case APP_MODE_ACCEL_TEST:
            error = doAccelTest();
            break;

        case APP_MODE_COUNT_TEST:
            error = doCountTest();
            break;

        default:
            error = ERR_APP_INVALID_MODE;
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
	if (m_current_mode == APP_MODE_WAVE) {
		waveTimerZeroHandler();
	}
	if (m_current_mode == APP_MODE_VLC) {
		vlcTimerHandler();
	}

}


/**
 * Interrupt handler for accelerometer interrupt
 */
ISR (INT1_vect)
{
	if (m_current_mode == APP_MODE_WAVE) {
		waveIntOneHandler();
	}
}
