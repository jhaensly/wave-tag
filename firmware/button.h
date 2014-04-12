/**
 * @file button.h
 *
 * @author Jason W Haensly <jason.haensly@gmail.com> 09 February 2014
 *
 * @copyright Copyright (c) 2014 Blinc Labs LLC
 * @copyright This software is licensed under the terms and conditions of the
 * MIT License. See LICENSE.md in the root directory for more information.
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdbool.h>
#include "module_id.h"

/**
 * Button events.
 */
enum button_event_t {
    BUTTON_SINGLE_PRESS, ///< A single press was detected
};


/**
 * Button states.
 */
enum button_state_t {
    BUTTON_PRESSED,     ///< Button is pressed
    BUTTON_RELEASED,    ///< Button is released
    BUTTON_DISABLED,    ///< Button is disabled
};


/**
 * Enable the button module.
 */
extern enum error_t buttonEnable(void);


/**
 * Disable the button module.
 */
extern enum error_t buttonDisable(void);


/**
 * @return The current state of the button.
 */
extern enum button_state_t buttonState(void);


/**
 * @return True if the button is released, false otherwise.
 */
#define BUTTON_RELEASED() (buttonState() == BUTTON_RELEASED)


/**
 * @return True if the button is pressed, false otherwise.
 */
#define BUTTON_PRESSED() (buttonState() == BUTTON_PRESSED)

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
extern void buttonSetCallback(void (* button_cb)(enum button_event_t event));


#endif // BUTTON_H_
