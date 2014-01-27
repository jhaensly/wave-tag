/**
 * @file vlc.h
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 26 Jan 2014
 *
 * VLC support for Wavetag DVT1
 */

#ifndef VLC_H_
#define VLC_H_

/**
* Enable VLC detection
*/
extern void enableVLC();

/**
* Disables VLC detection
*/	
extern void disableVLC();

/**
* Measure light value from LED.
*/	
extern uint8_t measureLED();


/**
* Performs interrupt operations for VLC mode
*/
extern void vlcTimerZeroHandler();

#endif