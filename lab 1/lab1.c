/*************************************************************
*
* Name:
* EGR 425 Lab 1 - Blink the LED that is attached to PIN13
*
**************************************************************/

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	/*
	 * DDRB is defined in C:\avr8-gnu-toolchain\avr\include\avr\iom328p.h
	 * with this statement: #define DDRB _SFR_IO8(0x04)
	 *
	 * _SFR_IO8 is defined in C:\avr8-gnu-toolchain\avr\include\avr\sfr_defs.h
	 * with this statement: #define _SFR_IO8(io_addr) ((io_addr) + __SFR_OFFSET)
	 * where __SFR_OFFSET is defined as 0x20
	 *
	 * So DDRB is the memory location of register DDRB.
	 * This is called Memory-Mapped-IO.
	 *
	 * You interact with the I/O device by doing reads/writes to the
	 * appropriate memory location of the registers.
	 *
	 */
	int wait = 50;
	for (int i = 0; i < 8; i ++) {
		DDRB |= (1 << i);
	}
	int N = 8;
	int RATE = 100;
	while (1) {
		// ========== Pattern 1 ==========

		PORTB |= (1 << 0);
		_delay_us( 100);
		PORTB &= ~(1 << 0);
		_delay_us( 100);
	}

	return 0;

}
