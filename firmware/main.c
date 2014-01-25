/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "accel_constants.h"

// Utility methods
typedef uint8_t bool;
#define true    1
#define false   0

#define BUSY_WHILE(c) while(c)
#define BUSY_UNTIL(c) BUSY_WHILE(!(c))
#define FIND_MIDPOINT(min, max) ((min) + (((max) - (min)) / 2))

// Edge detection sensitivity
#define LED_MEASUREMENT_SENSITIVITY (5)

// Debugging goodness
#define DEBUG

#ifdef DEBUG
#define OUTPUT_VALUE(v) do { PORTB = ~((v << 6) | (v >> 2)); } while(0)

#else
#define OUTPUT_VALUE(v)

#endif // DEBUG


volatile bool    is_button_down = false;
volatile uint8_t led_measurement_min;
volatile uint8_t led_measurement_max;
volatile uint8_t led_measurement_thresh;
volatile uint8_t led_measurement_time;
volatile uint8_t led_measurement_bit;

volatile uint8_t vlc_data[64];
volatile uint8_t vlc_data_idx = 0;

void twi_start(uint8_t addr) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));

    TWDR = addr;
    TWCR = (1<<TWINT)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));
}

void twi_send_byte(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    BUSY_UNTIL(TWCR & (1<<TWINT));
}

#define TWI_START_WRITE(a) twi_start(a)
#define TWI_START_READ(a)  twi_start((a) | 1)

// TODO: Generalize to I2C byte read
uint8_t readAccelReg(uint8_t reg) {
    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);

    TWI_START_READ(ACCEL_I2C_ADDR);

    TWCR = (1<<TWINT)|(1<<TWEN)|(0<<TWEA);		//wait for data; send NACK
    BUSY_UNTIL(TWCR & (1<<TWINT));

    TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);		//send stop
    return TWDR;
}

// TODO: Generalize to I2C byte write
void writeAccelReg(uint8_t reg, uint8_t val) {
    TWI_START_WRITE(ACCEL_I2C_ADDR);
    twi_send_byte(reg);
    twi_send_byte(val);

    TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);		//send stop
}


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


int main(void) {
    uint8_t i=0;

    DDRB  = 0xffu;    //LED PINS
    DDRD  = 0x00u;
    PORTD = 0x00u;

    OUTPUT_VALUE(5);

    //Timer0 interrupt
	//OCR0A = 50; //how high you count
	//TCCR0A = 0x0A; //compare match CTC, no multiplier
	//TIMSK0 = 0x02; //enable compare match interrupt.
	//TCNT0 = 0;

    //configure interrupt
    EICRA = 0x0cu; //rising edge
    EIMSK = 0x02u;
    sei();

    OUTPUT_VALUE(0);

    // Configure the accelerometer
    // Enable freefall detect on y; Event latch disable
    writeAccelReg(ACCEL_REG_FF_MT_CFG, 0x50u);
    // Enable freefall detect
    writeAccelReg(ACCEL_REG_INT_EN, 0x04u);
    // Freefall interrupt to INT2 pin
    writeAccelReg(ACCEL_REG_INT_PIN_MAP, 0x04u);
    // Push pull, active high
    writeAccelReg(ACCEL_REG_CTRL_3, 0x00u);
    // Set threshold. Max = 0x7f, which equals 8 G
    writeAccelReg(ACCEL_REG_FF_MT_THS, 0x0au);
    // Set ACTIVE bit to wake chip
    writeAccelReg(ACCEL_REG_CTRL_1, 0x01u);

    while(1) {
        //OUTPUT_VALUE(readAccelReg(ACCEL_OUT_Y_MSB));
        //_delay_ms(10);
        OUTPUT_VALUE(i++);
        _delay_ms(100);

        // TODO: Configure accel for inertial interrupt and put MCU to sleep
        //       when the device is inactive
    }
}

ISR (INT1_vect) {
   // _delay_ms(1);
   // OUTPUT_VALUE(0x00);
}


ISR(TIMER0_COMPA_vect) {
    if (PIND & 4) {
        // Button released
        is_button_down = false;
    }
    else {
        // Button pressed
        uint8_t led_measurement = measureLED();

        if (!is_button_down) {
            // Start the VLC measurement state machine. Reset measurement thresholds
            is_button_down = true;
            led_measurement_max     = led_measurement;
            led_measurement_min     = led_measurement;
            led_measurement_thresh  = led_measurement + (led_measurement/2);
            led_measurement_time    = 0;
            led_measurement_bit     = 0;
            return;
        }

        // Maximum and minimum values are primarily a function of the transmitter's distance from
        // the board. Since that is variable, find good values dynamically. At the start of a
        // transmission, they should change frequenctly. Later, as things stabilize, we should
        // jump to .
        if (led_measurement > led_measurement_max) {
            // Reset max threshold
            led_measurement_max = led_measurement;
            led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
        }
        else if (led_measurement < led_measurement_min)
        {
            // Reset min threshold
            led_measurement_min = led_measurement;
            led_measurement_thresh = FIND_MIDPOINT(led_measurement_min, led_measurement_max);
        }
        // At this point, we have established the dynamic range and set our threshold at the midpoint.
        // The VLC protocol encodes information on the duration between edges, so look for edges.
        else if ((led_measurement_bit == 0) &&
                 (led_measurement > (led_measurement_thresh + LED_MEASUREMENT_SENSITIVITY))) {
            // Handle rising edge
            led_measurement_bit = 1;
            OUTPUT_VALUE(led_measurement_time);
            led_measurement_time = 0;
        }

        else if((led_measurement_bit == 1) &&
                (led_measurement < (led_measurement_thresh - LED_MEASUREMENT_SENSITIVITY))) {
            // Handle falling edge
            led_measurement_bit = 0;
            OUTPUT_VALUE(led_measurement_time);
            led_measurement_time = 0;
        }
        // If no edge has been detected, increment the time. By doing this in the else, it saves us
        // from a particularly noisy initial measurement.
        else {
            led_measurement_time++;
        }
    }
}

