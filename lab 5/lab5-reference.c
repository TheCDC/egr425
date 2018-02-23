// this code continually scans ADC0 for an analog signal, using 16Mhz processor clock


#include <avr/io.h>
#include <stdint.h>       // needed for uint8_t

#include <avr/interrupt.h>
#include <util/delay.h>
#include "scale16.h"
#include "lcd.h"


volatile uint8_t ADCvalue;    // Global variable, set to volatile if used with ISR

void mydelay_ms(uint16_t count);

int main(void)
{
	lcd_init();

	ADMUX = 0;                // use ADC0
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	ADMUX |= (1 << ADLAR);    // Right adjust for 8 bit resolution

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE);   // Set ADC Auto Trigger Enable

	ADCSRB = 0;               // 0 for free running mode

	ADCSRA |= (1 << ADEN);    // Enable the ADC
	ADCSRA |= (1 << ADIE);    // Enable Interrupts

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion

	sei();    // Thanks N, forgot this the first time =P


	while (1)
	{
		// main loop

	}
}


ISR(ADC_vect)
{
	lcd_clear();
	ADCvalue = ADCH;          // only need to read the high value for 8 bit
	// REMEMBER: once ADCH is read the ADC will update

	// if you need the value of ADCH in multiple spots, read it into a register
	// and use the register and not the ADCH
	char out[10];
	sprintf(out, "%d", ADCvalue);
	lcd_write_string(out);
	mydelay_ms(100);

}

void mydelay_ms(uint16_t count) {
	while (count--) {
		_delay_ms(1);
	}
}