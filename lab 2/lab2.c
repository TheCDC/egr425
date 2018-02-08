
/*
 * Lab 2
 * Name: Christopher (Chen|Nugent)
 */

/* To run your program:
 * Step 1: make flash
 * Step 2: python -m serial.tools.miniterm --cr COM4 9600
 * Step 3: make sure you can receive data and send data
 */


#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"

//cpu frequency
#define CPU_F 16000000UL
//baud in bps
#define BAUD 9600
//formula to calculate baud rate. The clock signal at desired rate.
#define BAUDRATE (((F_CPU / (BAUD * 16UL))) - 1)

#define NOTE_DURATION     30720        /* determines note length */

const uint16_t notes[] = {
	G6, Fx6, A5, Gx5, E6, Gx6, C7, 0
};

//function prototypes
//implement these functions below
void USART_init(void);
char USART_receive(void);
void USART_send(char data);
void USART_send_string(char string[]);
void playNote(uint16_t wavelength, uint16_t duration);


// for (int i = 0; i < 8; i ++) {
// 	DDRD |= (1 << i);
// }

int main(void)
{
	for (int i = 0; i < 8; i ++) {
		DDRB |= (1 << i);
	}
	DDRD |= (1 << 7);
	USART_init();        //Call the USART initialization code
	char receivedChar;

	int index = 0 ;
	while (1) {
		receivedChar = USART_receive();
		USART_send(receivedChar);
		PORTD |= (1 << 7);
		_delay_ms( 1);
		PORTD &= ~(1 << 7);
		_delay_ms( 1);
		PORTD |= (1 << 7);
		_delay_ms( 1);
		PORTD &= ~(1 << 7);
		_delay_ms( 1);
		switch (receivedChar) {
		case 'z':
			index = 0;
			while (notes[index]) {
				PORTB |= (1 << index);
				if (index == 6) {
					PORTB |= 0b11111111;
				}
				for (int i = 0 ; i < 10; i++) {

					playNote(notes[index], 12000);
				}
				PORTB = 0;

				index++;
			}
			break;
		case 'x':
			break;
		case 'c':
			break;
		case 'v':
			break;
		}
		//make a switch statement, and based on the character you received do as described in the instructions
	}

	return 0;
}

void USART_init(void)
{
	//Step 1: set USART baud rate
	//Store in UBRR0 register. This register has High and Low register.
	//16-bit value is split into 2 registers - High and Low register
	UBRR0H = BAUDRATE >> 8;
	UBRR0L = BAUDRATE & 0b11111111;
	//Step 2: Enable USART Receiver and Transmitter
	UCSR0B |= 0b11 << 3;
	UCSR0C &= 0b11 << 1;
	//Step 3: use 8-bit data format

}


void USART_send(char data)
{
	//Step 1: check "USART Control and Status Register 0 A" register is set,
	//to make sure hardware is ready to send
	//it is checked with UDRE0 bit, which is a register that means UDR0 register is empty and new data can be transmitted

	//This is testing if the UDRE0 bit is set.
	//Assume UCSR0A to be 00100001
	//UDRE0 is bit 5:     00100000
	//ANDing these two:   00100000
	//this results in true, meaning bit 5 (UDRE0) is set.

	// wait until the proper bit is true
	while (!(UCSR0A & 1 << 5)) {
	}
	//Step 2: UDR0 is a special register to write data into. This data gets transmitted.
	UDR0 = data;

}

void USART_send_string(char string[])
{
	//in this function, you are to send an array of characters
	//send the characters one at a time using your existing USART_send function
	int index = 0;
	while (string[index]) {
		USART_send(string[index++]);

	}
}

char USART_receive(void)
{
	//Step 1: this is similar to USART_send
	//but now you are checking if the Receive Complete bit (7) is set.
	while (!(UCSR0A & 1 << 7)) {
	}


	//Step 2: Any received data is stored in UDR0 register.
	return UDR0 ;

}


//*** This is a working function as-is, but you are free to modify as you wish.
//Refer to scale16.h for valid wavelength values.
//Duration of the note can be the default NOTE_DURATION value
//or do integer division to shorten the duration of a note.
//This function takes a wavelength (inverse of a frequency) with a duration,
//and plays a note.
//It creates a note by sending a HIGH and LOW to the buzzer.
void playNote(uint16_t wavelength, uint16_t duration)
{
	uint16_t elapsed;
	uint16_t i;
	for (elapsed = 0; elapsed < duration; elapsed += wavelength) {
		// loop to wait for a precise amount of time
		for (i = 0; i < wavelength; i++) {
			_delay_us(1); //delay 1 micro second
		}
		//This is toggling the bit between 0 and 1.
		//1 << 7 because buzzer is connected in PortD's PIN7.
		//by sending ones and zeros very quickly in a loop,
		//a square wave is generated to make sound.
		PORTD ^= (1 << 7);
	}
}
