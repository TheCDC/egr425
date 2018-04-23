#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int dis;
volatile uint16_t adcValue = 0;
volatile float temperature = 0;

const uint16_t threshhold1 = 0;
const uint16_t threshhold2 = 0;



// Macros to make code meme-ier
#define OK 0
#define LIT 1
#define TOO_LIT 2
#define QUIT return

void mydelay_ms(uint16_t count);
void initADC();
void initTimer1Servo();
void initLED();
void setLED(int);
uint16_t readADC();

int main(void) {
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
    } else if (dis == LIT) {
        // turn on light
    } else if (dis == TOO_LIT) {
        // red alert
    }
}


void doStateChange() {
    // If dis is too lit, quit
    if (dis == TOO_LIT) {
        QUIT;
    }
    if (adcValue > threshhold2) {
        enterState(TOO_LIT);
    } else if (adcValue > threshhold1) {
        enterState(LIT);
    } else {
        // enterState(OK);
    }
}



ISR(BADISR_vect) {
    // enterState(OK);
    // doStateChange(adcValue);
}


ISR(ADC_vect) {
    ADCSRA &= ~(1 << ADIE);    // Disable Interrupts
    adcValue = (adcValue*3 + readADC())/4;
    // OCR1A = ((adcValue >> 3) << 6);
    ICR1 = adcValue << 5;

    ADCSRA |= (1 << ADIE);    // enable Interrupts
    float mvReading = (5000.0 / 1024.0) * adcValue;
	temperature = (mvReading - 500.0) / 10.0;
    // mydelay_ms(1);

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

