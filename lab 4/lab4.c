
/*
 * Lab 4
 * Name: Christopher (Chen|Nugent)
 */


#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"
#include "lcd.h"

#define GO_N 0
#define WAIT_N 1
#define GO_E 2
#define WAIT_E 3
#define WALKING_N 4
#define WALKING_E 5

#define NOTE_DURATION     30720
#define  A6      232


struct State {
	uint8_t Out; 	// 6-bit to output
	uint16_t Time; 	// delay in ms
	uint8_t Next[5];// next state for inputs 0,1,2,3
};


struct State FSM[6] = {							// East - North
	{0b0100001, 1000, {GO_N, WAIT_N, GO_N, WAIT_N, WAIT_N}}, // 100001 Red, Green
	{0b0100010, 500, {GO_E, GO_E, GO_E, GO_E, WALKING_E}},			// 100010 Red, Yellow
	{0b0001100, 1000, {GO_E, GO_E, WAIT_E, WAIT_E, WAIT_E}},		// 001100 Green, Red
	{0b0010100, 500, {GO_N, GO_N, GO_N, GO_N, WALKING_N}},			// 010100 Yellow, Red
	{0b1100100, 500, {GO_N, GO_N, GO_N, GO_N, WALKING_N}},
	{0b1100100, 500, {GO_E, GO_E, GO_E, GO_E, WALKING_E}},
};

#define PORT_LED PORTD
#define DDR_LED DDRD
#define PORT_BUTTON PORTC
#define DDR_BUTTON DDRC
#define PIN_BUTTON PINC
void mydelay_ms(uint16_t count);
void playNote(uint16_t wavelength, uint16_t duration);


uint8_t S; // current state

int main(void)
{
	lcd_init();
	lcd_clear();
	// Set D pins to output
	DDR_LED = 0b11111111;

	// Set B pins to input
	DDR_BUTTON = 0;

	// Enable pull-up resistors
	PORT_BUTTON = 0xFF;

	//initial state
	S = GO_N;

	//use this type when declaring 8-bit integers
	uint8_t i = 0;
	lcd_write_string("begin");
	while (1) {
		for (i = 0; i < 4; i++) { //why loop 4 times?
			PORT_LED = FSM[S].Out;     //output lights
			// PORTD = PINB;     //output lights
			mydelay_ms(FSM[S].Time);  //delay
			// while (!(DDRB & 1)) {}
			if ((PIN_BUTTON & 1) == 0) {
				S = FSM[S].Next[4];     //transition to next state
				lcd_clear();
				lcd_write_string("CROSS");
			}
			else {
				S = FSM[S].Next[i];     //transition to next state
				lcd_clear();
				lcd_write_string("DON'T CROSS");
			}
		}

	}
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
