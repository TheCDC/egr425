#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile int dis;


#define OK 0
#define LIT 1
#define TOO_LIT 2
#define QUIT return

void main() 
{

}

void doStateChange(ADC_VALUE) {
    // If dis is too lit, quit
    if (dis == TOO_LIT) {
        QUIT;
    }
    if (ADC_VALUE > threshhold2) {
        enterState(TOO_LIT);
    } else if (ADC_VALUE > threshhold1) {
        enterState(LIT);
    } else {
        enterState(OK);
    }
}

void enterState(int newState) {
    dis = newState;
    if (dis == OK) {
        // turn off light
    } else if (dis == LIT) {
        // turn on light
    } else if (dis == TOO_LIT) {
        // red alert
    }
}


ISR(/*button press */) {
    enterState(OK);
}

ISR(/* ADC ready */) {
    doStateChange(ADC_VALUE);
}