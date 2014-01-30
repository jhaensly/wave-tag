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

#include "util.h"
#include "alphabet.h"
#include "vlc_decoder_data.h"

// Edge detection sensitivity
#define LED_MEASUREMENT_SENSITIVITY (5)
#define TIME_MEASUREMENT_SENSITIVITY (3)

static uint8_t led_measurement_min;
static uint8_t led_measurement_max;
static uint8_t led_measurement_thresh;
static uint8_t led_measurement_time;
static uint8_t led_measurement_bit;
static bool preambleLock;
static uint8_t timeMin;
static uint8_t timeMax;
static uint8_t timeThreshold;
static uint8_t currentMessage;
static uint8_t positionCounter;
/**
* Measure light value from LED.
*/	
uint8_t measureLED() {
	PORTC = PORTC & 0xfcu; //kill both sides of the LED
	DDRC = 0x03u;

	//configure ADC
	ADMUX  = 0x61u; //left adjust, avcc ref, ADC1
	ADCSRA = 0x80u; //enable ADC, divide clock by 2

	//raise the cathode
	PORTC |= 0x01u;
	_delay_us(100);

	//take the LED anode out of the equation.
	DDRC = 0x01u;
	_delay_us(10);

	//START CONVERSION
	ADCSRA|=0b01000000;

	while (ADCSRA & 0b01000000) {
		ADCSRA |= (0b00010000);
	}

	uint8_t temp = ADCH;

	ADCSRA = 0x00u;  //disable ADC
	PORTC &= 0xf8u;
	DDRC   = 0x03u;  //return to normal
	ADMUX  = 0x00u;
	ADCSRA = 0x00u;
	return temp;
}	

/**
* Enable VLC detection
*/
void enableVLC() {
	//Timer0 interrupt
	preambleLock = false;
	OCR0A = 50; //how high you count
	TCCR0A = 0x0A; //compare match CTC, no multiplier
	TIMSK0 = 0x02; //enable compare match interrupt.
	TCNT0 = 0;
	led_measurement_max=led_measurement_min = measureLED();
	
    OUTPUT_VALUE(0X00);
	preambleLock=false;
    positionCounter=0;
	timeMin = 0xff;
	timeMax = 0x00;
	timeThreshold = 0x80;
	currentMessage = 0x00;
	sei();
	
	}

/**
* Disables VLC detection
*/	
void disableVLC() {
	TIMSK0 = 0;
}


/**
* Takes in time measurement. Returns
* letter index if valid, 0xff if not
*/
uint8_t isLetter(uint8_t time) {
    
    static uint8_t messageDepth;
    
    currentMessage<<=1;
	//OUTPUT_VALUE(currentMessage);
	if (time>timeThreshold)
	{
		currentMessage|=0x01;
	}
    uint8_t i;
    messageDepth = 4;
    currentMessage = 0x03;
    uint8_t decoderIndex = 0;
    for (i=0;i<messageDepth;i++)
    {
        uint8_t decoderResult;
        if (currentMessage&(1<<i))
            decoderIndex++;
        decoderResult = pgm_read_byte(&VLC_DECODER_DATA[decoderIndex]);
        if (decoderResult&0x80) {
            messageDepth=0;
            return decoderResult & 0x7F;
        }
        else
            decoderIndex = decoderResult;
    }
    messageDepth++;
    return 0xff;
}

/**
* Takes in a time measurement. Returns true if
* this byte finishes the preamble.
*/
bool isPreamble(uint8_t time)
{
	if (preambleLock)
		return false;

	if (timeMin > time) {
		timeMin = time;
		timeThreshold = FIND_MIDPOINT(timeMin, timeMax);
	}
	if (timeMax < time) {
		timeMax = time;
		timeThreshold = FIND_MIDPOINT(timeMin,timeMax);
	}
	

	
	currentMessage<<=1;
	//OUTPUT_VALUE(currentMessage);
	if (time>timeThreshold)
	{
		currentMessage|=0x01;
	}
	if ((currentMessage&0b11111)==0b10100) {
		preambleLock = true;
		//OUTPUT_VALUE(0xff);
		return true;
	}
	return false;
}

/**
* Performs interrupt operations for VLC mode
*/
void vlcTimerZeroHandler() {
	
	uint8_t led_measurement = measureLED();

	// Maximum and minimum values are primarily a function of the transmitter's distance from
	// the board. Since that is variable, find good values dynamically. At the start of a
	// transmission, they should change frequenctly. Later, as things stabilize, we should
	// jump to .
	if (led_measurement > led_measurement_max) {
		// Reset max threshold
		led_measurement_max = led_measurement;
		led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
	}
	else if (led_measurement < led_measurement_min) {			// Reset min threshold
		led_measurement_min = led_measurement;
		led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
	}
	// At this point, we have established the dynamic range and set our threshold at the midpoint.
	// The VLC protocol encodes information on the duration between edges, so look for edges.
	else if ((led_measurement_bit == 0) && 
		(led_measurement > (led_measurement_thresh + LED_MEASUREMENT_SENSITIVITY))) {
		// Handle rising edge
		led_measurement_bit = 1;
		
        if (isPreamble(led_measurement_time));
        {
            if (positionCounter<3)
                positionCounter++;
            else
            {
                OUTPUT_VALUE(0xffu);
                uint8_t tempLetter = isLetter(led_measurement_time);
                if (tempLetter != 0xff) {
                    OUTPUT_VALUE(tempLetter);
                }
            }
		}

		led_measurement_time = 0;
	}
	else if((led_measurement_bit == 1) &&
			(led_measurement < (led_measurement_thresh - LED_MEASUREMENT_SENSITIVITY))) {
		// Handle falling edge
		led_measurement_bit = 0;
		if (isPreamble(led_measurement_time));
        {
            if (positionCounter<3)
                positionCounter++;
            else
            {
                OUTPUT_VALUE(0xffu);
                uint8_t tempLetter = isLetter(led_measurement_time);
                if (tempLetter != 0xff) {
                    OUTPUT_VALUE(tempLetter);
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

