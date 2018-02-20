
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
	lcd_write_string("lolol");
}
