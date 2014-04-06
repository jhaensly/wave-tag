/**
 * @file sleep.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 07 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 *
 * Wrapper for the AVR sleep lib functions.
 *
 * The ATtiny48/88 define three low power sleep modes (from highest power
 * consumption to lowest):
 *   -SLEEP_MODE_IDLE
 *   -SLEEP_MODE_ADC
 *   -SLEEP_MODE_PWR_DOWN
 *
 * Idle mode will exit after any interrupt. There are no clock requirements
 * to enter this state.
 *
 * ADC mode draws less power than Idle and should improve the accuracy of ADC
 * measurements. To enter this state, the IO clock must be disabled. The IO
 * clock drives timers/counters, SPI, and external interrupts. (Interestingly,
 * because the TWI start condition is detected asynchronously, TWI address
 * recognition will still work with the IO clock disabled.)
 *
 * ADC noise reduction mode will only exit return after one of the following events:
 *   -ADC conversion complete interrupt
 *   -Watchdog system reset
 *   -Watchdog interrupt
 *   -Brown-out reset
 *   -TWI address match interrupt
 *   -EEPROM ready interrupt
 *   -External level interrupt on INT0 or INT1
 *   -Pin change interrupt
 *
 * In power down mode, the lowest power mode available, all clocks are stopped.
 * This mode will only exit after one of the following events:
 *   -External reset
 *   -Watchdog reset
 *   -Brown-out reset
 *   -Watchdog interrupt
 *   -TWI address match interrupt
 *   -EEPROM ready interrupt
 *   -External level interrupt on INT0 or INT1
 *   -Pin change interrupt
 *
 * Needless to say, entering one of the two lowest power modes without meeting the
 * stated clock requirements above requirements will almost certainly cause
 * strange and wonderful bugs to appear.
 *
 * Note that unless your purpose is to lock the CPU until reset, interrupts must
 * be enabled and configured before going to sleep.
 */

#ifndef SLEEP_H_
#define SLEEP_H_

#include <avr/sleep.h>
#include <stdbool.h>

/**
 * A function that defines when the sleep condition should be exited. Interrupts
 * will be disabled while this function executes.
 *
 * @return true while we should remain asleep, false when we should return.
 */
typedef bool (*sleep_while_condition_t)(void);


/**
 * Alias for the #defines in avr/sleep.h to permit compile time value checking.
 */
typedef enum {
    SLEEP_IDLE      = SLEEP_MODE_IDLE,      ///< Idle mode
    SLEEP_ADC       = SLEEP_MODE_ADC,       ///< ADC noise reduction mode
    SLEEP_PWR_DOWN  = SLEEP_MODE_PWR_DOWN,  ///< Power down mode
} sleep_mode_t;


/**
 * Enter the specified sleep mode until f_cond returns false.
 *
 * @param[in] mode The low power sleep state to enter.
 *
 * @param[in] f_cond A pointer to a function that will return false when the sleep
 * condition should be exited.
 */
extern void sleep(sleep_mode_t mode, sleep_while_condition_t f_cond);


#endif // SLEEP_H_
