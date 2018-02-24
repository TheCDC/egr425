
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
void initADC();

void writeFloat(float val);
uint16_t readADC();
float readTemp();
int16_t readLight();

int main(void)
{
	lcd_init();
	while (1) {
		initADC();

		float t = readTemp();
		uint16_t l = readLight();
		char out[20];
		lcd_write_string("Temp: ");
		writeFloat(t);
		lcd_write_string("    ");
		sprintf(out, "Light: %d", l);
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

	return r;
}

float readTemp() {
	ADMUX &= 0b11111000;
	uint16_t sensorReading = readADC();
    float mvReading = (5000.0/1024.0) * sensorReading;
	return (mvReading - 500.0) / 10.0;
}

int16_t readLight() {
	ADMUX |= 0b00000001;
	return readADC();
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

void writeFloat(float val) {
		int integer, decimal;	
		integer = val;
		decimal = (val - integer) * 10000;
		char out[10];
		sprintf(out, "%d.%d", integer, decimal);
		lcd_write_string(out);
}
