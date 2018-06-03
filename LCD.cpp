#include "LCD.hpp"
#include "utilities.h"

LCD_Drive::LCD_Drive()
{

}

void LCD_Drive::LCD_reset()
{
	I2C2& i2c = I2C2::getInstance();
	uint8_t Reg = 0x00;
	/*Initiate Reset Sequence for 4-bit mode interfacing*/
	i2c.writeReg(LCD_ID, Reg, 0x3C);//0x30
	i2c.writeReg(LCD_ID, Reg, 0x38);

	i2c.writeReg(LCD_ID, Reg, 0x3C);//0x30
	i2c.writeReg(LCD_ID, Reg, 0x38);

	i2c.writeReg(LCD_ID, Reg, 0x3C);//0x30
	i2c.writeReg(LCD_ID, Reg, 0x38);

	i2c.writeReg(LCD_ID, Reg, 0x2C);//0x20
	i2c.writeReg(LCD_ID, Reg, 0x28);

	delay_ms(10);

}
void LCD_Drive::LCD_init()
{
	/*Set function to 4-bit interfacing mode*/
	command_to_LCD(0x28); //0x28 Function set for 4-bit mode!

	/*DISPLAY AND CURSOR(0x0F)*/
	command_to_LCD(0x0C);//0x0F Display On, Cursor blink Off

	/*Display Clear(0x01)*/
	command_to_LCD(0x01);//0x01 Clear screen and cursor

	/*Entry Mode(0x06)*/
	command_to_LCD(0x06); //0x06 Entry mode, auto increment cursor

	/*Cursor Reset(0x80)*/
	command_to_LCD(0x80); //Set DDRAM address to ROW1(start)

	delay_ms(5);
}
void LCD_Drive::char_to_LCD(uint8_t character)
{
	I2C2& i2c = I2C2::getInstance();
	uint8_t byte,Reg = 0x00;
	/*Start with the upper nibble*/
	byte  = (character & 0xF0);
	byte |= BL;
	byte |= (E|RS);
	i2c.writeReg(LCD_ID, Reg, byte);
	byte &= ~(E|RS);
	byte |= BL;
	i2c.writeReg(LCD_ID, Reg, byte);
	delay_ms(3);
	/*Now do the lower nibble*/
	byte  = (character & 0x0F)<<4;
	byte |= BL;
	byte |= (E|RS);
	i2c.writeReg(LCD_ID, Reg, byte);
	byte &= ~(E|RS);
	byte |= BL;
	i2c.writeReg(LCD_ID, Reg, byte);
	delay_ms(3);
}
void LCD_Drive::string_to_LCD(char *myString)
{
 	uint8_t i = 0;
	 while(i != strnlen(myString, 16))
	   {
	      //delay_ms(50);
	      char_to_LCD(myString[i]);
	      i++;
	   }

}
void LCD_Drive::command_to_LCD(uint8_t command)
{
	I2C2& i2c = I2C2::getInstance();
	uint8_t byte,Reg = 0x00;
	/*Start with the upper nibble*/
	byte  = (command & 0xF0);
	byte |= BL;
	byte |= E;
	i2c.writeReg(LCD_ID, Reg, byte);
	byte &= ~E;
	byte |= BL;
	i2c.writeReg(LCD_ID, Reg, byte);
	/*Now do the lower nibble*/
	byte  = (command & 0x0F)<<4;
	byte |= BL;
	byte |= E;
	i2c.writeReg(LCD_ID, Reg, byte);
	byte &= ~E;
	byte |= BL;
	i2c.writeReg(LCD_ID, Reg, byte);
	//Delay to process commands
	delay_ms(3);
}

void LCD_Drive::arrow_up()
{
	command_to_LCD(0x80);  //LINE0
	char_to_LCD(0x7E);     //Print Arrow
	command_to_LCD(0xC0);  //LINE0
	string_to_LCD(" ");
}

void LCD_Drive::arrow_down()
{
	command_to_LCD(0xC0);  //LINE0
	char_to_LCD(0x7E);     //Print Arrow
	command_to_LCD(0x80);  //LINE0
	string_to_LCD(" ");
}

void LCD_Drive::Pi()
{
	command_to_LCD(0x01);
	command_to_LCD(0x80);
	string_to_LCD("QT");
	char_to_LCD(0xF7);
	string_to_LCD(" Edition!");
	command_to_LCD(0xC0);
	char_to_LCD(0xF7);
	string_to_LCD(" = 3.14159265359");	
	//string_to_LCD("||||||||||||||||");
}


LCD_Drive::~LCD_Drive()
{

}

