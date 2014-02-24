/**
 * @file vlc.c
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 26 Jan 2014
 *
 * VLC support for Wavetag DVT1
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "vlc.h"
#include "vlc_decoder_data.h"
#include "wave.h"
#include "util.h"
#include "display.h"
#include "adc.h"
#include "sleep.h"
#include "button.h"
#include "timer.h"

// Edge detection sensitivity
#define LED_MEASUREMENT_SENSITIVITY (3u)
#define TIME_MEASUREMENT_SENSITIVITY (2u)

static uint8_t led_measurement_min;
static uint8_t led_measurement_max;
static uint8_t led_measurement_thresh;
static uint8_t led_measurement_time;
static uint8_t led_measurement_bit;
static bool    preambleLock;
static uint8_t timeMin;
static uint8_t timeMax;
static uint8_t timeThreshold;
static uint8_t currentMessage;
static uint8_t currentMessageLength;

//used in several places to keep track of how
//many bits have been transmitted.
static uint8_t positionCounter;

static uint8_t cursorLocation;

static uint8_t messageDepth;


/* Takes in time measurement. Returns
 * letter index if valid, 0xff if not */
static uint8_t isLetter(uint8_t time) {
    currentMessage<<=1;
	if (time>timeThreshold) {
		currentMessage|=0x01;
	}
    uint8_t i;
    uint8_t decoderIndex = 0;
    //counting on i to roll under
    for (i=messageDepth;i<255;i--) {
        uint8_t decoderResult;
        if (currentMessage&(1<<i))
            decoderIndex++;
        decoderResult = pgm_read_byte(&VLC_DECODER_DATA[decoderIndex]);
        if (decoderResult&0x80) {
            messageDepth=0;
            //displayByte(currentMessage);
            return decoderResult & 0x7F;
        }
        else {
            decoderIndex = decoderResult;
        }
    }
    messageDepth++;
    return 0xff;
}

static bool vlcActive(void) {
    return BUTTON_PRESSED();
}

/**
 * Measure light value from LED.
 */
uint8_t measureLED() {
	PORTC = PORTC & 0xfcu; //kill both sides of the LED
	DDRC = 0x03u;
    
	//raise the cathode
	PORTC |= 0x01u;
	_delay_us(100);
    
	//take the LED anode out of the equation.
	DDRC = 0x01u;
	_delay_us(10);
    
	//START CONVERSION
	ADCSRA|=0b01000000;
	while (ADCSRA & 0b01000000) {
		//ADCSRA |= (0b00010000);
	}
    
	uint8_t temp = ADCH;
    
	
	PORTC &= 0xf8u;
	DDRC   = 0x00u;  //return to normal

	return temp;
}


/*
 * Takes in a time measurement. Returns true if
 * this byte finishes the preamble.
 */
static bool isPreamble(uint8_t time) {
	if (preambleLock) {
		return true;
    }

	positionCounter++;
	if (timeMin > time) {
		timeMin = time;

		timeThreshold = FIND_MIDPOINT(timeMin, timeMax);
		return false;
	}
	if (timeMax < time) {
		timeMax = time;
		timeThreshold = FIND_MIDPOINT(timeMin,timeMax);
		return false;
	}

	currentMessage<<=1;

	if (time>timeThreshold) {
		currentMessage|=0x01;
	}
	
	if (((currentMessage&0b11111)==0b10100)&&(positionCounter>5)) {
		preambleLock = true;
		positionCounter=0;
		currentMessage=0;
	}
	return false;
}

static void vlcTimerCb(void) {
    
    uint8_t led_measurement = measureLED();
    
    
    
	// Maximum and minimum values are primarily a function of the transmitter's distance from
	// the board. Since that is variable, find good values dynamically. At the start of a
	// transmission, they should change frequenctly. Later, as things stabilize, we should
	// jump to .

	if (led_measurement > led_measurement_max) {
		// Reset max threshold
		led_measurement_max = led_measurement;
		led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
		return;
	}
	else if (led_measurement < led_measurement_min) {			// Reset min threshold
		led_measurement_min = led_measurement;
		led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
		return;
	}
	// At this point, we have established the dynamic range and set our threshold at the midpoint.
	// The VLC protocol encodes information on the duration between edges, so look for edges.

	else if (((led_measurement_bit == 0) && (led_measurement > (led_measurement_thresh + LED_MEASUREMENT_SENSITIVITY)))
	||((led_measurement_bit == 1) && (led_measurement < (led_measurement_thresh - LED_MEASUREMENT_SENSITIVITY)))) {
		// Handle rising edge
		led_measurement_bit^=1;

        if (isPreamble(led_measurement_time))
        {
		uint8_t tempLetter = isLetter(led_measurement_time);
			if (positionCounter<3)
				positionCounter++;
			else {
				if (tempLetter != 0xff) {
					displayByte(tempLetter);
					outputText[cursorLocation]=tempLetter;
                    currentMessageLength++;
					cursorLocation++;
				}
			}
		}

		led_measurement_time = 0;
	}

	// If no edge has been detected, increment the time. By doing this in the else, it saves us
	// from a particularly noisy initial measurement.
	else {
		led_measurement_time++;
	}
}

error_t vlcReceive() {
    error_t err = ERR_NONE;

    displayEnable();
    currentMessageLength=0;
	cursorLocation=0;
	preambleLock = false;
	led_measurement_min = UINT8_MAX;
	led_measurement_max = 0;
	led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
    messageDepth = 0;
    positionCounter=0;
	timeMin = 0xff;
	timeMax = 0x00;
	timeThreshold = 0x80;
	currentMessage = 0x00;
    
    adcEnable(ADC_CHAN_1);
    err = timer0Start(&vlcTimerCb,250,true);

    if (err == ERR_NONE) {
        sleep(SLEEP_MODE_IDLE, &vlcActive);
    }
    
    timer0Stop();
    adcDisable();

    for (int i=currentMessageLength;i<MESSAGE_LENGTH;i++) {
        outputText[i]=0;
    }

    PORTC  &= 0xf8;
    DDRC    = 0x03;

	refreshFrameBuffer();
    displayDisable();

    return err;
}
