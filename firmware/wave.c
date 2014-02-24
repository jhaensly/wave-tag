/**
 * @file wave.c
 *
 * @author Michael Ciuffo <ch0000f@gmail.com> 24 Jan 2014
 *
 * Font for Wavetag DVT1
 */

#include "wave.h"
#include "glyph.h"
#include "accel.h"
#include <avr/interrupt.h>
#include "display.h"
#include "timer.h"

/**
 * Matrix of output messages.
 */
volatile uint8_t outputText[MESSAGE_LENGTH]={};

/**
 * Width of display output in pixels.
 */
#define FRAME_BUFFER_LENGTH 10

volatile bool is_wave_active;

/**
 * Pixel framebuffer of current thing to display.
 */
volatile uint8_t frameBuffer [FRAME_BUFFER_LENGTH];

static uint8_t currentLetterLength;

//ignore the first few pulses so you don't prematurely jump into display mode.
static volatile uint8_t ignoreShakes = 2;

/**
 * Location of column "cursor" that is editing the array when adding columns.
*/
static volatile uint8_t frameBufferCursor;

//stores current letter being displayed
static volatile uint8_t messageCursor;

/**
 * Present duration of current vertical column.
 */
static volatile uint8_t columnTimer;

/**
 * Elapsed time of current wave cycle.
 */
static volatile uint32_t waveTimer;

/**
 * The pixel column currently being displayed
 */
static volatile uint8_t currentColumnNumber;

/**
 * Length of time to display each column. Calculated from waveTime.
 */
static volatile uint8_t columnTime;

/**
 * Timeout between accelerometer interrupt and start of message display.
 */
static volatile uint32_t blackoutDelay;



/**
* Records current direction of device.  Important because
* accelerometer pin is symmetrical.
*/
static bool goingRight;

/**
* How many hardware interrupts since you last had to reset counters
*/
static uint8_t interruptCount;


/**
 * Add letter to end of frame buffer.
 */
static void addLetter(uint8_t letter)
{
	currentLetterLength = pgm_read_byte(&GLYPH_WIDTH[letter]);
	uint8_t currentLetterPosition = pgm_read_byte(&GLYPH_IDX[letter]);
	uint8_t j;
	for (j=0;j<currentLetterLength;j++)
	{
		frameBuffer[frameBufferCursor]=pgm_read_byte(&GLYPH_DATA[currentLetterPosition+j]);
		frameBufferCursor++;
	}

	//Add gaps between letters
	frameBuffer[frameBufferCursor]=0x00u;
	frameBufferCursor++;

}

/**
 * Regenerate framebuffer from text string.
 */
void refreshFrameBuffer()
{

	//uint8_t i;
	//for(i=0;i<MESSAGE_LENGTH;i++)
	//{
	if (messageCursor<MESSAGE_LENGTH) {
		frameBufferCursor=0;
		///@todo fix this for however we're storing messages
		addLetter(outputText[messageCursor]);
	}
	//while (frameBufferCursor<SIZEOF_ARRAY(frameBuffer))
	//{
	//	frameBuffer[frameBufferCursor]=0xffu;
	//	frameBufferCursor++;
	//}
	//frameBufferCursor = 0;
}

/**
 * Print item from current column of frame buffer to LEDs.
 */
static void printCol(uint8_t col)
{
	displayByte(frameBuffer[col]);
	return;
}

/**
* Disable timers used for display.
*/
void killDisplay() {
    timer0Stop();
    accelDisable();
    displayDisable();
}

static void waveInactive(void) {
    if (++waveTimer >= DISPLAY_SLEEP_TIMEOUT) {
        is_wave_active = false;
        timer0Stop();
    }
}

static void waveText(void) {
    waveTimer++;
    if (columnTimer < columnTime) {
        columnTimer++;
    }
    else {
        columnTimer = 0;
        //move to next row. +2 handles space between letters
        if (currentColumnNumber < (currentLetterLength+1)) {
            printCol(currentColumnNumber);
            currentColumnNumber++;
        }
        else {
            if (++messageCursor < MESSAGE_LENGTH) {
                currentColumnNumber=0;
                refreshFrameBuffer();
            }
            else {
                timer0Restart(&waveInactive);
            }
        }
    }
}

static void waveBlackout(void) {
    if (++waveTimer > blackoutDelay) {
        timer0Restart(&waveText);
    }
}

/**
 * Same shit but for external interrupt 1.
 */
void waveIntOneHandler() {
	uint32_t nextWaveTime;

    accel_data_t val;
    accelReadValue(ACCEL_Y, &val);
    if (val>0)
    {
        nextWaveTime = waveTimer;
        blackoutDelay = (nextWaveTime>>2)+(nextWaveTime>>4);
        columnTime = ((nextWaveTime)-(blackoutDelay))/(150);
        waveTimer=0;

        //reset timers once per cycle
        currentColumnNumber = 0;
        messageCursor=0;
        displayByte(0x00);
        refreshFrameBuffer();

        goingRight=false;
        interruptCount=0;
        columnTimer=0;

        timer0Restart(&waveBlackout);
    }

	//}
	///@todo clear interrupt flag in case there were any spurious interrupts during this vector
	//GIFR = (1<<PCIF0);

}

/**
 * Initialize timers and interrupts required for displaying text.
 */
void initDisplay() {
	///@todo bring this out into a timer .h file
    waveTimer = 0;
    is_wave_active = true;

    displayEnable();

    // Order here matters. If accelerometer interrupt fires before the timer is
    // initalized, the wave state machine could get out of sync.
    timer0Start(&waveInactive, 50, true);
    accelEnableFreefall(&waveIntOneHandler);
}

