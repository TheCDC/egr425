
/*
 * Lab 5
 * Name: Christopher (Chen|Nugent)
 */


#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"
#include "lcd.h"

#define NOTE_DURATION     30720
#define  A6      232

#define PORT_LED PORTD
#define DDR_LED DDRD


void mydelay_ms(uint16_t count);
void playNote(uint16_t wavelength, uint16_t duration);
void initADC() ;
uint16_t readADC();


int main(void)
{
	lcd_init();
	while (1) {
		initADC();

		uint16_t x = readADC();
		char out[10];
		sprintf(out, "%d", x);
		lcd_write_string(out);
		mydelay_ms(100);
		lcd_clear();
	}

}

void initADC() {
	ADMUX = 0;                // use ADC0
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	// ADMUX |= (1 << ADLAR);    // Right adjust for 8 bit resolution

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	// ADCSRA |= (1 << ADATE);   // Set ADC Auto Trigger Enable

	// ADCSRB = 0;               // 0 for free running mode

	ADCSRA |= (1 << ADEN);    // Enable the ADC
	ADCSRA |= (1 << ADIE);    // Enable Interrupts

	// ADCSRA |= (1 << ADSC);    // Start the ADC conversion

}

uint16_t readADC() {
	uint16_t r;
	// set ADSC bit to start measuring
	ADCSRA |= 1 << 6;
	while (ADCSRA & (1 << 6)) {

	}
	uint16_t l = ADCL;
	r = l;
	r = ((ADCH) << 8) | l;
;
	return r;
}
void mydelay_ms(uint16_t count) {
	while (count--) {
		_delay_ms(1);
	}
}

void playNote(uint16_t wavelength, uint16_t duration)
{
	uint16_t elapsed;
	uint16_t i;
	for (elapsed = 0; elapsed < duration; elapsed += wavelength) {
		// loop to wait for a precise amount of time
		for (i = 0; i < wavelength; i++) {
			_delay_us(1); //delay 1 micro second
		}
		PORTD ^= (1 << 7);
	}
}
