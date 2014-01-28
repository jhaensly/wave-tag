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
 * Ticks per microsecond.
 *
 * F_CPU is the system clock rate in Hz. It is defined in the Makefile based on
 * the fuse configuration.
 */
#define TICKS_PER_MICROSECOND   (F_CPU / 1000000UL)

/**
 * Ticks per millisecond.
 *
 * F_CPU is the system clock rate in Hz. It is defined in the Makefile based on
 * the fuse configuration.
 */
#define TICKS_PER_MILLISECOND   (F_CPU / 1000UL)

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
 * @param[in]  ticks The timer duration in ticks.
 * @param[in]  recurring True if the timer should be reinserted immediately.
 * @return     error_t
 */
extern error_t timer0Start(handle_timer_expired_t cb,
                           uint8_t ticks,
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
extern error_t timer0Stop(void);


/**
 * Start a timer.
 *
 * @param[in]  cb The callback to execute when the timer expires.
 * @param[in]  ticks The timer duration in ticks.
 * @param[in]  recurring True if the timer should be reinserted immediately.
 * @return     error_t
 */
extern error_t timer1Start(handle_timer_expired_t cb,
                           uint16_t ticks,
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

#endif // TIMER_H_
