// Lab 6 - Chris (Chen|Nugent)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void mydelay_ms(uint16_t count);


int main(void) {

	sei();

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

