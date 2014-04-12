/**
 * @file main.c
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * @todo module description
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "accel.h"
#include "wave.h"
#include "vlc.h"
#include "module_id.h"
#include "sleep.h"
#include "display.h"
#include "button.h"
#include "timer.h"
#include "adc.h"

static void appError(enum error_t err_code);
static enum error_t appSleep(void);
static enum error_t appWaiting(void);
static enum error_t appVLC(void);
static enum error_t appWave(void);
static enum error_t appAccelTest(void);
static enum error_t appCountTest(void);
static enum error_t appTimerTest(void);

static volatile enum {
    APP_MODE_SLEEP,
    APP_MODE_WAITING,
    APP_MODE_VLC,
    APP_MODE_WAVE,
    APP_MODE_ACCEL_TEST,
    APP_MODE_COUNT_TEST,
    APP_MODE_TIMER_TEST,

    APP_MODE_COUNT          /* This must remain last */
} m_current_mode,
  m_next_mode;

/*
 * This array of app modes must be in the same order as the array of enums
 * defined above. Main uses the enum value as an index into this array.
 */
typedef enum error_t (* handle_app_mode_t)(void);
static const handle_app_mode_t m_app_mode_handler[] = {
    appSleep,
    appWaiting,
    appVLC,
    appWave,
    appAccelTest,
    appCountTest,
    appTimerTest
};

void appError(enum error_t err_code)
{
    static const uint8_t fw_ver = (APP_VERSION_MAJOR << 4) | APP_VERSION_MINOR;

    displayEnable();
    for (int i=0; i<10; i++) {
        displayByte(err_code);
        _delay_ms(100);

        displayByte(fw_ver);
        _delay_ms(100);
    }
    displayDisable();

    /** @todo Reset device */
}


static bool isButtonUp(void)
{
    return BUTTON_RELEASED();
}

static enum error_t appSleep(void)
{
    sleep(SLEEP_PWR_DOWN, &isButtonUp);

    //m_next_mode = BUTTON_RELEASED() ? APP_MODE_WAVE : APP_MODE_VLC;
    m_next_mode = APP_MODE_WAVE;

    displayEnable();
    for (int i=0; i<4; i++) {
        displayByte(0x1);
        _delay_ms(5);
        displayByte(0);
        _delay_ms(5);
    }

    displayDisable();
    return ERR_NONE;
}

static enum error_t appWaiting(void)
{
    return ERR_APP_INVALID_MODE;
}

static enum error_t appVLC(void)
{
    enum error_t err = vlcReceive();

	m_next_mode = APP_MODE_WAVE;
	return err;
}

static bool appIsWaveActive(void)
{
    return is_wave_active && BUTTON_RELEASED();
}

static enum error_t appWave(void)
{
	//while button is pressed and held, stay in VLC mode
	initDisplay();
	refreshFrameBuffer();

    sleep(SLEEP_IDLE, appIsWaveActive);

	killDisplay();
	m_next_mode = BUTTON_PRESSED() ? APP_MODE_VLC : APP_MODE_SLEEP;
	return ERR_NONE;
}

static enum error_t appAccelTest(void)
{
    enum error_t err;
    displayEnable();
    do {
        int8_t val;
        err = accelReadValue(ACCEL_Y, &val);
        displayByte(val);
        _delay_ms(10);
    } while (m_current_mode == m_next_mode &&
             err == ERR_NONE);

    displayDisable();
    return err;
}

static enum error_t appCountTest(void)
{
    uint8_t i = 0;
    displayEnable();

    while(m_current_mode == m_next_mode) {
        i++;
        displayByte(i);
        _delay_ms(10);
    }

    displayDisable();
    return ERR_NONE;
}

static volatile uint8_t m_timer_counter;
static void appTimerTestCB(void)
{
    m_timer_counter++;
}

static enum error_t appTimerTest(void)
{
    m_timer_counter = 0xf0;
    enum error_t err;

    if ((err = displayEnable()) != ERR_NONE) {
        return err;
    }

    if ((err = timer0Start(appTimerTestCB, 200, true)) != ERR_NONE) {
        return err;
    }

    while (m_current_mode == m_next_mode) {
        displayByte(m_timer_counter);
    }

    err = displayDisable();
    return err;
}

int main(void)
{
    /* We're not using SPI or analog comparitor, so disable them to save power. */
    PRR  |= _BV(PRSPI);
    ACSR |= _BV(ACD);

    buttonEnable();
    accelDisable();
    displayDisable();

    timerInit();
    adcInit();

    m_current_mode  = APP_MODE_SLEEP;
    m_next_mode     = APP_MODE_WAVE;

    memset((uint8_t*)outputText, 1, sizeof(outputText));

    sei();

    while(1) {
        enum error_t error;

        if (m_current_mode < APP_MODE_COUNT) {
            error = m_app_mode_handler[m_current_mode]();
        } else {
            error = ERR_APP_INVALID_MODE;
        }

        if (error != ERR_NONE) {
            appError(error);
        }

        m_current_mode = m_next_mode;
    }
}
