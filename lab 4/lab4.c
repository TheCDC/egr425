
/*
 * Lab 4
 * Name:
 */


#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"
#include "lcd.h"

#define NOTE_DURATION     30720        /* determines note length */



int main(void) {
	lcd_init();
	lcd_clear();
	while (1) {

		lcd_write_string("wololo");
		_delay_ms(500);
		lcd_clear();
		_delay_ms(500);
		lcd_write_string("test");
		_delay_ms(500);
		lcd_clear();
		_delay_ms(500);

	}
}
