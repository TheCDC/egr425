// Lab 6 - Chris (Chen|Nugent)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void mydelay_ms(uint16_t count);
void initADC();
uint16_t readADC();
uint16_t adcValue = 0;
uint16_t c = 0;

int main(void) {
	initADC();
	initTimer1Servo();
	sei();
	while (1) {
		for (int i = 0 ; i < (1 << 8); i += 2) {
			OCR1A = ((adcValue >> 3) << 6);
			mydelay_ms(10);

		}

	}

}




void mydelay_ms(uint16_t count) {
	while (count--) {
		_delay_ms(1);
	}
}

uint16_t readADC() {
	uint16_t r;
	uint16_t l = ADCL;
	r = l;
	r = ((ADCH) << 8) | l;

	return r;
}

void initTimer1Servo(void) {
	// Set fast PWM mode
	TCCR1A |= (1 << WGM11);
	// set to count to max value
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	// prescaler 16 Mhz / 8 = 2,000,000
	TCCR1B |= (1 << CS11);
	// Set to non-inverting PWM mode
	TCCR1A |= (1 << COM1A1);
	// Set OCR1A to threshold value
	OCR1A = 2800;
	// Set PWM frequency
	ICR1 = 39999;
	// enable PORTB bit position 1 == DigitalPin9
	DDRB |= (1 << 1);
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


ISR(ADC_vect) {
	ADCSRA &= ~(1 << ADIE);    // Disable Interrupts
	adcValue = readADC();
	ADCSRA |= (1 << ADIE);    // enable Interrupts
	// lcd_clear();
	// lcd_write_string("Release to read light...");
	// mydelay_ms(1);

}
