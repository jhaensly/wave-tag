/**
 * @file module_id.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 26 Jan 2014
 *
 * Define unique ids for each module in the system. Valid values range
 * from 0x00 - 0x0f.
 *
 */

#ifndef MODULE_ID_H_
#define MODULE_ID_H_

typedef enum {
    MODULE_ID_INVALID       = 0x00, ///< Reserved
    MODULE_ID_APP           = 0x01, ///< WaveTag application
    MODULE_ID_TIMER         = 0x02, ///< Timer
    MODULE_ID_ACCEL         = 0x03, ///< Accelerometer
} module_id_t;

typedef enum {
    /// No error. In fact, success!
    ERR_NONE                = 0x00,

    /// Invalid application mode
    ERR_APP_INVALID_MODE    = 0x00 | MODULE_ID_APP,

    /// NULL callback
    ERR_TIMER_INVALID_PARAM = 0x00 | MODULE_ID_TIMER,
    /// All timers currently in use
    ERR_TIMER_BUSY          = 0x10 | MODULE_ID_TIMER,
    /// NULL timer
    ERR_TIMER_STOPPED       = 0x20 | MODULE_ID_TIMER,

    /// Invalid argument
    ERR_ACCEL_INVALID_ARG   = 0x00 | MODULE_ID_ACCEL,
} error_t;


#endif // MODULE_ID_H_
