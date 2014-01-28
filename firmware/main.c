/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#include "accel.h"
#include "alphabet.h"
#include "display.h"
#include "module_id.h"
#include "timer.h"
#include "util.h"
#include "vlc.h"

#define BUTTON_RELEASED()   (PIND & 4)
#define BUTTON_PRESSED()    (!BUTTON_RELEASED())


static volatile enum {
    APP_MODE_SLEEP,
    APP_MODE_WAITING,
    APP_MODE_VLC,
    APP_MODE_WAVE,
    APP_MODE_ACCEL_TEST,
    APP_MODE_COUNT_TEST,
    APP_MODE_TIMER_TEST,
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
    error_t err = ERR_NONE;
    err = enableVLC();
    if (err != ERR_NONE) {
        return err;
    }

	_delay_us(100);
	BUSY_UNTIL(BUTTON_RELEASED());
	err = disableVLC();

	m_next_mode = APP_MODE_WAVE;
	return err;
}

static error_t doWave(void) {
#if 0
	//while button is pressed and held, stay in VLC mode
	initDisplay();
	outputText[0]=1;
	outputText[1]=2;
	outputText[2]=3;
	refreshFrameBuffer();

	BUSY_UNTIL(BUTTON_PRESSED());

	killDisplay();
	m_next_mode = APP_MODE_VLC;
#endif
	return ERR_NONE;
}

static error_t doAccelTest(void) {
    error_t err = ERR_NONE;
    while ((m_current_mode == m_next_mode) &&
           (err == ERR_NONE)) {
        accel_data_t val;
        err = accelReadValue(ACCEL_Y, &val);
        OUTPUT_VALUE(val);
        _delay_ms(10);
    }

    return err;
}

static error_t doCountTest(void) {
    uint8_t i = 0;
    while(1) {
        i++;
        OUTPUT_VALUE(i);
        _delay_ms(10);
    }
    return ERR_NONE;
}

static volatile uint8_t m_timer_test_counter;
static void incrementCounter(void) {
    m_timer_test_counter++;
}

static error_t doTimerTest(void) {
    timer0Start(&incrementCounter, UINT8_MAX, true);
    while(m_current_mode == m_next_mode) {
        OUTPUT_VALUE(m_timer_test_counter);
    }
    timer0Stop();
    return ERR_NONE;
}

int main(void) {
    DDRB  = 0xffu;    //LED PINS
    DDRD  = 0x00u;
    PORTD = 0x00u;
	OUTPUT_VALUE(0X00u);

    accelConfigFreefall();
    timerInit();
    m_current_mode  = APP_MODE_TIMER_TEST;
    m_next_mode     = APP_MODE_TIMER_TEST;

    sei();
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

        case APP_MODE_TIMER_TEST:
            error = doTimerTest();
            break;

        default:
            error = ERR_APP_INVALID_MODE;
            break;
        }

        if (error != ERR_NONE) {
            error_state(error);
        }

        m_current_mode = m_next_mode;
    }
}


#if 0
/**
 * Interrupt handler for timer.  In charge of displaying text.
 */
ISR(TIMER0_COMPA_vect) {
	if (m_current_mode == APP_MODE_WAVE) {
		waveTimerZeroHandler();
	}
	if (m_current_mode == APP_MODE_VLC) {
		vlcTimerZeroHandler();
	}
}
#endif

/**
 * Interrupt handler for accelerometer interrupt
 */
ISR (INT1_vect)
{
	if (m_current_mode == APP_MODE_WAVE) {
		waveIntOneHandler();
	}
}

