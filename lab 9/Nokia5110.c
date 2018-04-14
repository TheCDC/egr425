// Nokia5110.c

// Font table, initialization, and other functions based
// off of Nokia_5110_Example from Spark Fun:
// 7-17-2011
// Spark Fun Electronics 2011
// Nathan Seidle
// http://dlnmh9ip6v2uc.cloudfront.net/datasheets/LCD/Monochrome/Nokia_5110_Example.pde

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

//Modified original file to use Nokia5110 on ATmega328p

#include "Nokia5110.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>


/*
 * LCD's port
 */
#define PORT_LCD PORTB
#define DDR_LCD DDRB

/*
 * LCD's pins
 */
//#define LCD_SCE 0
//#define LCD_RST 1
//#define LCD_DC 3
//#define LCD_DIN 4
//#define LCD_CLK 5

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


enum typeOfWrite{
  COMMAND,                              // the transmission is an LCD command
  DATA                                  // the transmission is data
};

char Screen[504]; // buffer stores the next image to be printed on the screen



// This is a helper function that sends an 8-bit message to the LCD.
// inputs: type     COMMAND or DATA
//         message  8-bit code to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
void static lcdwrite(enum typeOfWrite type, char message){
  if(type == COMMAND){
    write(message, 0);
  } else{
    write(message, 1);
  }
}

//Initialize LCD for use
void nokia_lcd_init(void)
{
  register unsigned i;
  /* Set pins as output */
  DDR_LCD |= 1<<LCD_SCE | 1<<LCD_RST | 1<<LCD_DC | 1<<LCD_DIN | 1<<LCD_CLK;

  /* Reset display */
  PORT_LCD |= (1 << LCD_RST);
  PORT_LCD &= ~(1 << LCD_SCE);
  _delay_ms(10);
  PORT_LCD &= ~(1 << LCD_RST);
  _delay_ms(70);
  PORT_LCD |= (1 << LCD_RST);

  //Enable SPI hardware
  SPCR |=1<<SPE | 1<<MSTR | 1<<SPR0;

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
  for (i = 0; i < 504; i++)
    write_data(0xFF);

  /* Activate LCD */
  write_cmd(0x08);
  write_cmd(0x0C);
}

void write(uint8_t bytes, uint8_t is_data)
{
  // Enable controller
  //PORT_LCD &= ~(1 << LCD_SCE);

  // We are sending data
  if (is_data) {
    PORT_LCD &= ~(1 << LCD_SCE);
    PORT_LCD |= (1 << LCD_DC);
  }
  // We are sending commands
  else {
    PORT_LCD &= ~(1 << LCD_DC);
    PORT_LCD &= ~(1 << LCD_SCE);
  }

  SPDR = bytes;                                  //Load data into buffer
  while ( !(SPSR & (1<<SPIF)) );

  // Disable controller 
  PORT_LCD |= (1 << LCD_SCE);
}

void write_cmd(uint8_t cmd)
{
  write(cmd, 0);
}

void write_data(uint8_t data)
{
  write(data, 1);
}

void nokia_lcd_clear(void)
{
  int i;
  for(i=0; i<504; i++){
    Screen[i] = 0; 
  }

}

void nokia_lcd_fill(void)
{
  int i;
  for(i=0; i<504; i++){ 
    Screen[i] = 0xFF; 
  }
}


void nokia_lcd_render(void)
{
  register unsigned i;
  // Set column and row to 0 
  write_cmd(0x80);
  write_cmd(0x40);

  // Write screen to display 
  for (i = 0; i < 504; i++)
    write_data(Screen[i]);
}




//********Nokia5110_PrintBMP*****************
// Bitmaps defined above were created for the LM3S1968 or
// LM3S8962's 4-bit grayscale OLED display.  They also
// still contain their header data and may contain padding
// to preserve 4-byte alignment.  This function takes a
// bitmap in the previously described format and puts its
// image data in the proper location in the buffer so the
// image will appear on the screen after the next call to
//   Nokia5110_DisplayBuffer();
// The interface and operation of this process is modeled
// after RIT128x96x4_BMP(x, y, image);
// inputs: xpos      horizontal position of bottom left corner of image, columns from the left edge
//                     must be less than 84
//                     0 is on the left; 82 is near the right
//         ypos      vertical position of bottom left corner of image, rows from the top edge
//                     must be less than 48
//                     2 is near the top; 47 is at the bottom
//         ptr       pointer to a 16 color BMP image
//         threshold grayscale colors above this number make corresponding pixel 'on'
//                     0 to 14
//                     0 is fine for ships, explosions, projectiles, and bunkers
// outputs: none
void Nokia5110_PrintBMP(uint16_t xpos, uint16_t ypos, const unsigned char *ptr, uint16_t threshold){
  uint16_t width = ptr[18], height = ptr[22], i, j;
  uint16_t screenx, screeny;
  unsigned char mask;
  // check for clipping
  if((height <= 0) ||              // bitmap is unexpectedly encoded in top-to-bottom pixel order
     ((width%2) != 0) ||           // must be even number of columns
     ((xpos + width) > SCREENW) || // right side cut off
     (ypos < (height - 1)) ||      // top cut off
     (ypos > SCREENH))           { // bottom cut off
    return;
  }
  if(threshold > 14){
    threshold = 14;             // only full 'on' turns pixel on
  }
  // bitmaps are encoded backwards, so start at the bottom left corner of the image
  screeny = ypos/8;
  screenx = xpos + SCREENW*screeny;

  mask = ypos%8;                // row 0 to 7
  mask = 0x01<<mask;            // now stores a mask 0x01 to 0x80
  j = ptr[10];                  // byte 10 contains the offset where image data can be found
  for(i=1; i<=(width*height/2); i=i+1){
    // the left pixel is in the upper 4 bits
    if(((ptr[j]>>4)&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;

    // the right pixel is in the lower 4 bits
    if((ptr[j]&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;

    j = j + 1;
    if((i%(width/2)) == 0){     // at the end of a row
      if(mask > 0x01){
        mask = mask>>1;
      } else{
        mask = 0x80;
        screeny = screeny - 1;
      }
      screenx = xpos + SCREENW*screeny;

      // bitmaps are 32-bit word aligned
      switch((width/2)%4){      // skip any padding
        case 0: j = j + 0; break;
        case 1: j = j + 3; break;
        case 2: j = j + 2; break;
        case 3: j = j + 1; break;
      }
    }
  }
}
// There is a buffer in RAM that holds one screen
// This routine clears this buffer
void Nokia5110_ClearBuffer(void){int i;
  for(i=0; i<SCREENW*SCREENH/8; i=i+1){
    Screen[i] = 0;              // clear buffer
  }
}


//********Nokia5110_DisplayBuffer*****************
// Fill the whole screen by drawing a 48x84 screen image.
// inputs: none
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DisplayBuffer(void){
  Nokia5110_DrawFullImage(Screen);
}


//********Nokia5110_OutChar*****************
// Print a character to the Nokia 5110 48x84 LCD.  The
// character will be printed at the current cursor position,
// the cursor will automatically be updated, and it will
// wrap to the next row or back to the top if necessary.
// One blank column of pixels will be printed on either side
// of the character for readability.  Since characters are 8
// pixels tall and 5 pixels wide, 12 characters fit per row,
// and there are six rows.
// inputs: data  character to print
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutChar(unsigned char data){
  int i;
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
  for(i=0; i<5; i=i+1){
    lcdwrite(DATA, ASCII[data - 0x20][i]);
  }
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
}

//********Nokia5110_OutString*****************
// Print a string of characters to the Nokia 5110 48x84 LCD.
// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutString(char *ptr){
  while(*ptr){
    Nokia5110_OutChar((unsigned char)*ptr);
    ptr = ptr + 1;
  }
}

//********Nokia5110_OutUDec*****************
// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
// Inputs: n  16-bit unsigned number
// Outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutUDec(unsigned short n){
  if(n < 10){
    Nokia5110_OutString("    ");
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutString("   ");
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutString("  ");
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else if(n<10000){
    Nokia5110_OutChar(' ');
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else {
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
}

//********Nokia5110_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next character will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=11)
//         newY  new Y-position of the cursor (0<=newY<=5)
// outputs: none
void Nokia5110_SetCursor(unsigned char newX, unsigned char newY){
  if((newX > 11) || (newY > 5)){        // bad input
    return;                             // do nothing
  }
  // multiply newX by 7 because each character is 7 columns wide
  lcdwrite(COMMAND, 0x80|(newX*7));     // setting bit 7 updates X-position
  lcdwrite(COMMAND, 0x40|newY);         // setting bit 6 updates Y-position
}

//********Nokia5110_Clear*****************
// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to (0,0) (top left corner of screen).
// inputs: none
// outputs: none
void Nokia5110_Clear(void){
  int i;
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, 0x00);
  }
  Nokia5110_SetCursor(0, 0);
}

//********Nokia5110_DrawFullImage*****************
// Fill the whole screen by drawing a 48x84 bitmap image.
// inputs: ptr  pointer to 504 byte bitmap
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DrawFullImage(const char *ptr){
  int i;
  Nokia5110_SetCursor(0, 0);
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, ptr[i]);
  }
}



