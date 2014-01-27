/**
 * @file alphabet.h
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 24 Jan 2014
 *
 * Font for Wavetag DVT1
 */

#ifndef ALPHABET_H_
#define ALPHABET_H_

#include <avr/pgmspace.h>

/**
 * Number of characters in output.
 */
#define MESSAGE_LENGTH 12

/**
 * Time elapsed during inactivity before reverting back to menu state.
 */
#define displayRefreshTimeout 5000

/**
 * Initialize display for showing text.
 */

extern void initDisplay();

/**
 * Pump text from outputText array to display.
 * @todo Add ability to pass in text array directly to function.
 */
extern void refreshFrameBuffer();

/**
 * Do stuff in timer0 ISR here because AVRdude says you can't do it like a normal person.
 */
extern void waveTimerZeroHandler();

/**
 * Same shit but for external interrupt 1.
 */
extern void waveIntOneHandler();

/**
 * Matrix of output messages.
 */
extern volatile uint8_t outputText[MESSAGE_LENGTH];

/**
* Disable timers used for display.
*/
void killDisplay();

#endif // ALPHABET_H_