// Lab 6 - Chris (Chen|Nugent)
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/interrupt.h>

int showScreenSaver = 0;
int adcValue = 0;
int state = -1;
int frame = 0;
void initInterrupt();
void initADC();
void mydelay_ms(uint16_t count);
void writeFloat(float val);
uint16_t readADC();
int16_t readLight();


int main(void) {
	initADC();
	// Set B pins to input
	DDRD = 0x00;

	// Enable pull-up resistors
	PORTD |= (1 << 2) | (1 << 3);
	initInterrupt();
	lcd_init();
	lcd_clear();
	while (1) {
		frame ++;
		if (state == -1) {
			lcd_clear();

			lcd_write_string("Ready");
			mydelay_ms(50);
		}
		else if (state == 0) {
			lcd_write_char("-\\|/*"[frame % 5]);
			// lcd_write_string("main");
			mydelay_ms(50);
		}
		else if (state == 1) {
			lcd_clear();
			char out[20];
			sprintf(out, "Light: %d", adcValue);
			lcd_write_string(out);
			// lcd_write_string("ADC");
			mydelay_ms(50);
		}
	}
}


void initADC() {
	ADMUX = 0;                // use ADC0
	ADMUX |= (1 << REFS0);    // use AVcc as the reference

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE);   // Set ADC Auto Trigger Enable

	ADCSRB = 0;               // 0 for free running mode

	ADCSRA |= (1 << ADEN);    // Enable the ADC
	ADCSRA |= (1 << ADIE);    // Enable Interrupts

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion


}

void initInterrupt() {
	EIMSK |= (1 << INT0) | (1 << INT1);
	EICRA &= 0b0000;
	sei();    // Enable interrupts


}


ISR(INT0_vect) {
	state = 1;
	lcd_clear();

	lcd_write_string("INT0");

	ADCSRA |= (1 << ADIE);    // Enable Interrupts

}

ISR(INT1_vect) {
	lcd_clear();
	if (state == 0) {
		state = 2;
	} else {
		state = 0;
	}
}

ISR(ADC_vect) {
	ADCSRA &= ~(1 << ADIE);    // Disable Interrupts
	adcValue = readADC();
	// lcd_clear();
	// lcd_write_string("Release to read light...");
	// mydelay_ms(1);

}

void mydelay_ms(uint16_t count) {
	while (count--) {
		_delay_ms(1);
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
uint16_t readADC() {
	uint16_t r;
	uint16_t l = ADCL;
	r = l;
	r = ((ADCH) << 8) | l;

	return r;
}

int16_t readLight() {
	ADMUX |= 0b00000001;
	return readADC();
}
