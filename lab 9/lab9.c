/* Lab 9 - Game and EEPROM */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "Nokia5110.h"
#include <stdint.h>            // has to be added to use uint8_t




// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10
const unsigned char SmallEnemy[] = {
	0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};


// a missile in flight
// includes one blacked out row on the top, bottom, and right of the image to prevent smearing when moved 1 pixel down, up, or left
// width=4 x height=9
const unsigned char MissileImg[] = {
	0x42, 0x4D, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00,
	0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};

// small explosion best used for the demise of an enemy
// width=16 x height=10
const unsigned char SmallExplosion[] = {
	0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
	0x0F, 0x00, 0x0F, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0xF0, 0xF0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};

// image of the player's ship
// includes two blacked out columns on the left and right sides of the image to prevent smearing when moved 2 pixels to the left or right
// width=18 x height=8
const unsigned char PlayerShip[] = {
	0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xAA, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};


struct State {
	unsigned long x;      // x coordinate
	unsigned long y;      // y coordinate
	const unsigned char *image; // pointer to images
	long life;            // 0=dead, 1=alive
};

typedef struct State STyp;

STyp Enemy[4];
STyp Player;
STyp Missile;

volatile int state;
volatile int button;
int* button_ptr = &button;

void mydelay_ms(uint16_t count);


// Atmega328 interrupts
// https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328

void initButtonPins() {
	// Set B pins to input
	DDRD = 0x00;

	// Enable pull-up resistors
	PORTD |= (1 << DDB2);
	PORTD |= (1 << DDB3);
	PORTD |= (1 << DDB4);
}

void initButtonInterrupt() {
	// EIMSK |= (1 << INT2);
	// EICRA &= 0b0000;
	PCICR |= (1 << PCIE2);    // set PCIE2 to enable PCMSK0 scan
	PCMSK2 |= (1 << PCINT18);
	PCMSK2 |= (1 << PCINT19);
	// PCMSK2 |= (1 << PCINT20);
	sei();    // Enable interrupts
}


int main() {
	state = 97;
	button = 0;

	initButtonPins();
	initButtonInterrupt();

	nokia_lcd_init();
	nokia_lcd_clear();
	// nokia_lcd_fill();
	Nokia5110_OutString("lcd is alive");
	nokia_lcd_render();
	while (1) {
		nokia_lcd_clear();
		nokia_lcd_render();
		char out[10] = {0};
		sprintf(out, "%d", button);

		Nokia5110_OutString(out);
		// nokia_lcd_render();
		button ++;
		state ++;
		mydelay_ms(100);

	}
}

ISR(PCINT18_vect) {
	cli();
	Nokia5110_OutString("18");
	*button_ptr = 18;
	button = 18;
	if ( (PIND & (1 << PIND2)) == 1 )
	{
		/* LOW to HIGH pin change */

	}
	else
	{
		/* HIGH to LOW pin change */
		// state = 0;
	}
	sei();
}


void mydelay_ms(uint16_t count) {
	while (count--) {
		_delay_ms(1);
	}
}