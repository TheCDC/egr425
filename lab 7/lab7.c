// Lab 6 - Chris (Chen|Nugent)
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

int showScreenSaver = 0;
int adcValue = 0;
int state = -1;
int frame = 0;
char animation[5] = "-\\|/*";
void mydelay_ms(uint16_t count);
void writeFloat(float val);
uint16_t readADC();
int16_t readLight();
void pin2_isr();

int hasWritten = 0;
uint16_t timerValue = 0;
int isTiming = 1;



void initTimer1(float s) {
	TCNT1 = 0;
	TCCR1B |= (1 << CS02) | (1 << CS00);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 15625 * s;
	sei();
}

// void initTimer0() {
// 	TCNT0 = 0;
// 	TCCR0B |= (1 << CS02) | (1 << CS00);
// 	// TCCR0B = 0;
// 	// TCCR0B &= ~(1 << CS02) & ~(1 << CS01) & ~(1 << CS00);

// 	TIMSK0 |= (1 << OCIE0A);
// 	OCR0A = 15625;
// 	sei();
// }


void initButtonInterrupt() {
	EIMSK |= (1 << INT0) | (1 << INT1);
	EICRA &= 0b0000;
	sei();    // Enable interrupts
}


int main(void) {
	// Set B pins to input
	DDRD = 0x00;

	// Enable pull-up resistors
	PORTD |= (1 << 2) | (1 << 3);
	initTimer1(0.1);
	initButtonInterrupt();
	lcd_init();
	lcd_clear();
	lcd_write_string("ready");
	sei();
	int repeated = 0;
	while (1) {
		// if (isTiming == 1) {

		lcd_clear();
		char out[10];
		sprintf(out, "%d.%d", timerValue / 10, timerValue % 10);
		lcd_write_string(out);
		// }
		_delay_ms(50);
	}
}



ISR(INT0_vect) {
	// lcd_write_string("INT0");
	// disable stopwatch
	TCNT1 = 0;
	isTiming = 0;
	OCR1A = 15625 * 4;
	// initTimer1(4);
	// _delay_ms(50);
	// enable sleep timer
}

ISR(INT1_vect) {
	// lcd_write_string("INT1");
	// initTimer1(0.1);
	TCNT1 = 0;
	OCR1A = 15625 / 10;
	isTiming = 1;
	// _delay_ms(50);
}

ISR(TIMER1_COMPA_vect) {  //interrupt triggered every 0.25s
	if (isTiming) {
		TCNT1 = 0;
		timerValue++;
		if (timerValue >= 600) {
			timerValue = 0;
		}

	} else {
		// disable timers
		TCNT0 = 0;
		TCCR1B &= ~(1 << CS02) & ~(1 << CS01) & ~(1 << CS00);
		TCCR0B &= ~(1 << CS02) & ~(1 << CS01) & ~(1 << CS00);

		lcd_write_string("sleep");
		sleep_enable();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		cli();
		sleep_bod_disable();
		sei();
		lcd_clear();
		sleep_cpu();
		initTimer1(0.1);
	}
}

// ISR(TIMER0_COMPA_vect) {  //interrupt triggered every 0.25s

// }
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
