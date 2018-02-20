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


/*
 * Must be called once before any other function, initializes display
 */
void lcd_init();

/*
 * Clear screen
 */
void lcd_clear();


/**
 * Draw single char 
 * @code: char code
 */
void lcd_write_char(char code);

/**
 * Draw string
 * @string: sending string
 */
void lcd_write_string(char string[]);



