/**
 * @file timer.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 25 Jan 2014
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "module_id.h"


/**
 * Function to be called when a timer expires.
 *
 * @warning This will be called from an ISR, so design accordingly.
 */
typedef void (*handle_timer_expired_t)(void);


/**
 * Initialize the timer module.
 *
 * @return error_t
 */
extern error_t timerInit(void);


/**
 * Start a timer.
 *
 * @param[in]  cb The callback to execute when the timer expires.
 * @param[in]  usec The timer duration in microseconds.
 * @param[in]  recurring True if the timer should be reinserted immediately.
 * @return     error_t
 */
extern error_t timer0Start(handle_timer_expired_t cb,
                           uint8_t usec,
                           bool recurring);

/**
 * Cancel a timer.
 *
 * @return error_t
 *
 * @warning This operation is nowhere near atomic, so beware race conditions.
 * Users should set their internal state such that a timer expiring before this
 * call completes is handled gracefully.
 */
extern error_t timer0Stop(void);


/**
 * Restart a timer.
 *
 * @param[in]  cb The callback to execute when the timer expires. The can be the
 * same callback as was set initially.
 *
 * @return error_t
 */
extern error_t timer0Restart(handle_timer_expired_t cb);

#if 0
/**
 * Start a timer with millisecond resolution.
 *
 * @param[in]  cb The callback to execute when the timer expires.
 * @param[in]  msec The timer duration in milliseconds.
 * @param[in]  recurring True if the timer should be restarted immediately.
 * @return     error_t
 */
extern error_t timer1Start(handle_timer_expired_t cb,
                           uint16_t msec,
                           bool recurring);

/**
 * Cancel a timer
 *
 * @return error_t
 *
 * @warning This operation is nowhere near atomic, so beware race conditions.
 * Users should set their internal state such that a timer expiring before this
 * call completes is handled gracefully.
 */
extern error_t timer1Stop(void);


/**
 * Restart a timer.
 *
 * @return error_t
 */
extern error_t timer1Restart(void);
#endif

#endif // TIMER_H_
