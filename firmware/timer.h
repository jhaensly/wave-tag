/**
 * @file timer.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "module_id.h"


/**
 * Initialize the timer module.
 *
 * @return enum error_t
 */
extern enum error_t timerInit(void);


/**
 * Start a timer.
 *
 * @param[in]  timer_cb The callback to execute when the timer expires.
 * @param[in]  usec The timer duration in microseconds.
 * @param[in]  recurring True if the timer should be reinserted immediately.
 * @return     enum error_t
 */
extern enum error_t timer0Start(void (* timer_cb)(void),
                                uint8_t usec,
                                bool recurring);

/**
 * Cancel a timer.
 *
 * @return enum error_t
 *
 * @warning This operation is nowhere near atomic, so beware race conditions.
 * Users should set their internal state such that a timer expiring before this
 * call completes is handled gracefully.
 */
extern enum error_t timer0Stop(void);


/**
 * Restart a timer.
 *
 * @param[in]  timer_cb The callback to execute when the timer expires. The can
 * be the same callback as was set initially.
 *
 * @return enum error_t
 */
extern enum error_t timer0Restart(void (* timer_cb)(void));

#if 0
/**
 * Start a timer with millisecond resolution.
 *
 * @param[in]  timer_cb The callback to execute when the timer expires.
 * @param[in]  msec The timer duration in milliseconds.
 * @param[in]  recurring True if the timer should be restarted immediately.
 * @return     enum error_t
 */
extern enum error_t timer1Start(void (* timer_cb)(void),
                                uint16_t msec,
                                bool recurring);

/**
 * Cancel a timer
 *
 * @return enum error_t
 *
 * @warning This operation is nowhere near atomic, so beware race conditions.
 * Users should set their internal state such that a timer expiring before this
 * call completes is handled gracefully.
 */
extern enum error_t timer1Stop(void);


/**
 * Restart a timer.
 *
 * @return enum error_t
 */
extern enum error_t timer1Restart(void);
#endif

#endif // TIMER_H_
