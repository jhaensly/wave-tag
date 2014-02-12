/**
 * @file vlc.h
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 26 Jan 2014
 *
 * VLC support for Wavetag DVT1
 */

#ifndef VLC_H_
#define VLC_H_

#include "module_id.h"

/**
 * Enable VLC detection
 */
extern error_t vlcEnable(void);

/**
 * Disables VLC detection
 */
extern error_t vlcDisable(void);

#if 0
/**
 * Measure light value.
 */
extern uint8_t vlcTakeMeasurement(void);

/**
 * Performs interrupt operations for VLC mode
 */
extern void vlcTimerHandler(void);
#endif

#endif
