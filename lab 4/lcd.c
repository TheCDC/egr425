#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_chars.h"

/*
 * LCD's port
 */
#define PORT_LCD PORTB
#define DDR_LCD DDRB

/*
 * LCD's pins
 */

//SERIAL CHIP ENABLE to SLAVE SELECT : default pin 10
#define LCD_SCE 2

//SIGNAL RESET for operation of LCD
#define LCD_RST 0

//DATA OR COMMAND
#define LCD_DC 1

//DATA IN TO SLAVE always pin 11 (MOSI)
#define LCD_DIN 3

//clock line always pin 13 (SERIAL CLOCK)
#define LCD_CLK 5

#define LCD_CONTRAST 0x40



void lcd_init(void)
{

	/* TODO : Set appropriate pins as output as discussed in lecture */
	//Add your code here!
	//1. Set data direction register
	DDR_LCD |=   1 << LCD_SCE | 1 << LCD_RST | 1 << LCD_DC | 1 << LCD_DIN | 1 << LCD_CLK;
	//2. Reset display
	PORT_LCD |= (1 << LCD_RST);
	PORT_LCD &= ~(1 << LCD_SCE);
	_delay_ms(50);
	PORT_LCD &= ~(1 << LCD_RST);
	_delay_ms(100);
	PORT_LCD |= (1 << LCD_RST);
	//3. Enable SPI
	SPCR |= 1 << SPE | 1 << MSTR | 1 << SPR0;



	/*
	 * Initialize display
	 */
	/* Enable controller */
	PORT_LCD &= ~(1 << LCD_SCE);

	/* -LCD Extended Commands mode- */
	write_cmd(0x21);

	/* LCD bias mode 1:48 */
	write_cmd(0x13);

	/* Set temperature coefficient */
	write_cmd(0x06);

	/* Default VOP (3.06 + 66 * 0.06 = 7V) */
	write_cmd(0xC2);

	/* Standard Commands mode, powered down */
	write_cmd(0x20);

	/* LCD in normal mode */
	write_cmd(0x09);

	/* Clear LCD RAM */
	write_cmd(0x80);

	write_cmd(LCD_CONTRAST);

	/* Activate LCD */
	write_cmd(0x08);
	write_cmd(0x0C);
}



void lcd_clear()
{
	//TODO clear LCD screen
	for (int i = 0; i < 84; i++) {
		write_data(0);
	}
}

void lcd_fill(void)
{
	//TODO fill LCD screen
}

void lcd_write_char(char code)
{
	uint8_t letter = (uint8_t)code - 0x20;	// Get array location
	uint8_t i;
	for (i = 0; i < 5; i++) {	// 5 bytes per character
		write_data(CHARS[letter][i]);
	}
}

void lcd_write_string(char string[])
{
	//TODO write string (use lcd_write_char)
	int i = 0;
	while (string[i]) {
		lcd_write_char(string[i++]);
	}
}


void write_cmd(uint8_t cmd)
{
	// 1. Set D/C to command mode (low)
	PORT_LCD &= ~(1 << LCD_DC);

	// 2. Set SCE low
	PORT_LCD &= ~(1 << LCD_SCE);

	// 3. Use SPI to load data to register and wait until transmission complete
	SPDR = cmd;
	while (!(SPSR & (1 << SPIF)) );

	// 4. Set SCE high
	PORT_LCD |= (1 << LCD_SCE);
}


void write_data(uint8_t data)
{
	//TODO
	// 1. Set D/C to data mode (high)
	PORT_LCD |= (1 << LCD_DC);

	// 2. Set SCE low
	PORT_LCD &= ~(1 << LCD_SCE);

	// 3. Use SPI to load data to register and wait until transmission complete
	SPDR = data;
	while (!(SPSR & (1 << SPIF)) );
	// 4. Set SCE high
	PORT_LCD |= (1 << LCD_SCE);
}

