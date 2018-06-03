#ifndef LCD_H
#define LCD_H

#include "LPC17xx.h"
#include "i2c2.hpp"
#include "string.h"

#define LCD_ID 0x7e    //New device ID(New LCD Poggers)
/*Command shortcuts*/
#define LINE1  0x80     //Starting address for screen ROW 0, Column 0 
#define LINE2  0xC0     //Starting address for screen ROW 1, Column 0
#define Clear 0x01     //Clear contents on screen and reset cursor to ROW0, Column 0.

/*Define Control bit registers*/
#define RS 0x01        //D0 - Read(0)/Write(1) mode to internal DDRAM
#define RW 0x02        //D1 - Read Busy Flag
#define E  0x04        //D2 - Enable for pulsing
#define BL 0x08 //D3 - Backlight of LCD (1)ON (0) OFF

/**/

/*

PINOUT for ze backpack and LCD POGGERS
P7 -> D7
P6 -> D6
P5 -> D5
P4 -> D4
n/c-> D3 (P3)-> BackLight
n/c-> D2 (P2)-> E
n/c-> D1 (P1)-> RW
n/c-> D0 (P0)-> RS

Thus we must use 4-bit mode!

P0 -> RS
P1 -> RW
P2 -> E
P3 -> Backlight(BL)

|P7|P6|P5|P4|P3|P2|P1|P0| I2C Backpack I/0 Pins 
|D7|D6|D5|D4|D3|D2|D1|D0| LCD Module   I/O Pins		
|  D A T A  |BL|E |RW|RS| Control Pins
*/

class LCD_Drive
{
private:

public:
      LCD_Drive();

	void LCD_init();	
	void LCD_reset();
	void char_to_LCD(uint8_t character);
	void string_to_LCD(char *myString); 
	void command_to_LCD(uint8_t command);
	void arrow_up();
	void arrow_down();
	void Pi();
 

      ~LCD_Drive();
};

#endif
