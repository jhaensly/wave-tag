/**
 * @file button.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 09 February 2014
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdbool.h>
#include "module_id.h"

/**
 * Button events.
 */
typedef enum {
    BUTTON_SINGLE_PRESS, ///< A single press was detected
} button_event_t;


/**
 * Button states.
 */
typedef enum {
    BUTTON_PRESSED,     ///< Button is pressed
    BUTTON_RELEASED,    ///< Button is released
    BUTTON_DISABLED,    ///< Button is disabled
} button_state_t;


/**
 * Enable the button module.
 */
extern error_t buttonEnable(void);


/**
 * Disable the button module.
 */
extern error_t buttonDisable(void);


/**
 * @return The current state of the button.
 */
extern button_state_t buttonState(void);


/**
 * @return True if the button is released, false otherwise.
 */
#define BUTTON_RELEASED() (buttonState() == BUTTON_RELEASED)


/**
 * @return True if the button is pressed, false otherwise.
 */
#define BUTTON_PRESSED() (buttonState() == BUTTON_PRESSED)


/**
 * A button callback type definition.
 */
typedef void (*button_cb_t)(button_event_t);


/**
 * A callback to be executed on a button event.
 *
 * @param[in] cb The callback to be executed. Only the most recently set
 * callback will be triggered.
 *
 * @warning This callback will be executed from an ISR, so design accordingly.
 *
 * @todo Improve button event detection.
 */
extern void buttonSetCallback(button_cb_t cb);


#endif // BUTTON_H_
