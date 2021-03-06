#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int dis;
volatile uint16_t adcValue = 0;
volatile float temperature = 0;
volatile uint8_t WUBWUB = 0;

const uint16_t threshhold1 = 25;
const uint16_t threshhold2 = 30;



// Macros to make code meme-ier
#define OK 0
#define LIT 1
#define TOO_LIT 2
#define QUIT return

void mydelay_ms(uint16_t count);
void initButtonInterrupt();
void initADC();
void initTimer1Servo();
void initLED();
void setLED(int);
uint16_t readADC();

int main(void) {
	initButtonInterrupt();
	initLED();
	initUSART();
	initADC();
	initTimer1Servo();
	sei();
	while (1) {
		printWord((int)temperature);
		printString("\n");
		mydelay_ms(200);
	}
}

void enterState(int newState) {
	if (dis == newState) {
		return;
	}
	dis = newState;
	if (dis == OK) {
		// turn off light
		setLED(0);
		WUBWUB = 0;
	} else if (dis == LIT) {
		// turn on light
		setLED(1);
		WUBWUB = 0;
	} else if (dis == TOO_LIT) {
		// red alert
		setLED(1);
		WUBWUB = 1;
	}
}


void doStateChange() {
	// If dis is too lit, quit
	// if (dis == TOO_LIT) {
	// 	QUIT;
	// }
	if (temperature > threshhold2) {
		enterState(TOO_LIT);
	} else if (temperature > threshhold1) {
		enterState(LIT);
	} else {
		enterState(OK);
	}
}


ISR(BADISR_vect) {
	// uint8_t prev = all_buttons;
	// all_buttons = PIND & all_buttons;
	// printWord(100);
	WUBWUB = 0;
}



ISR(ADC_vect) {
	ADCSRA &= ~(1 << ADIE);    // Disable Interrupts
	adcValue = (adcValue * 3 + readADC()) / 4;
	// OCR1A = ((adcValue >> 3) << 6);
	if (WUBWUB) {
		ICR1 = adcValue << 5;
	}
	else {
		ICR1 = 0;
	}

	ADCSRA |= (1 << ADIE);    // enable Interrupts
	float mvReading = (5000.0 / 1024.0) * adcValue;
	temperature = (mvReading - 500.0) / 10.0;
	// mydelay_ms(1);

	// ========== State Transitions ==========
	doStateChange();

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

void initButtonInterrupt() {
	// EIMSK |= (1 << INT2);
	// EICRA &= 0b0000;
		// Set B pins to input
	DDRD = 0x00;

	// Enable pull-up resistors
	PORTD |= (1 << DDB2); // digital pin 2
	PCICR |= (1 << PCIE2);    // set PCIE2 to enable PCMSK0 scan
	PCMSK2 |= (1 << PCINT18);
	sei();    // Enable interrupts
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

void initLED(void) {
	DDRB |= (1 << 0);

}

void setLED(int enabled) {
	if (enabled) {
		PORTB |= (1 << 0);
	} else {
		PORTB &= ~(1 << 0);
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

