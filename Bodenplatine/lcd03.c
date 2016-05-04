// lcd03.c: Displaybibliothek

////////////
// Header //
////////////

#include <stdlib.h>
#include <string.h>
#include "twimaster.h"
#include "lcd03.h"

////////////////
// Funktionen //
////////////////

// String anzeigen
void display_output_string(uint8_t zeile, uint8_t spalte, char text[])
{
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		i2c_write(3);
		i2c_write(zeile);
  		i2c_write(spalte);
		for(uint8_t i=0; text[i]!=0; ++i)
			i2c_write(text[i]);
		i2c_stop();                           
	}
}

// Integer anzeigen
void display_output_int(uint8_t zeile, uint8_t spalte, uint16_t zahl)
{
	char text[6];
    itoa(zahl, text, 10);
	display_output_string(zeile, spalte, text);
}

// Zahl anzeigen
void display_output_num(uint8_t zeile, uint8_t spalte, uint16_t zahl, uint8_t num)
{
	char text[17];
    itoa(zahl, text, num);  
	display_output_string(zeile, spalte, text);
}

// Display leeren
void clear_lcd(void)
{
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		i2c_write(12);
		i2c_stop();
	}
}

// Cursor versetzen
void cursor_x_y(uint8_t zeile, uint8_t spalte)
{
    if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		i2c_write(3);
		i2c_write(zeile);
		i2c_write(spalte);		
		i2c_stop(); 
	}
}

// Cursor verändern
void cursor_sichtbar(enum E_CURSOR cur)
{
    if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		if(!cur)
			i2c_write(4);
		else if(cur==2)
			i2c_write(5);
		else
			i2c_write(6);
		i2c_stop();
	}
}

// Beleuchtung steuern
void lcd_light(uint8_t on)
{
    if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		if(on)
			i2c_write(19);
		else
			i2c_write(20);
		i2c_stop();  
	}
}

// einzelnes Zeichen schreiben
void write_char(unsigned char c) {
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		i2c_write(c);
		i2c_stop();
	}
}

// eigenes Zeichen senden
void lcd_my_char(char num, char data[8])
{
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		i2c_write(27);
		i2c_write(num);
		for(uint8_t i=0; i<8; ++i)
			i2c_write(data[i]);
		i2c_stop();
	}
}

// String ausgeben
void lprint(const char *s)
{
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(0);
		for(uint8_t i=0; s[i]!='\0'; ++i) {
			i2c_write(s[i]);
		}
		i2c_stop();
	}
}

// Zeichen von Keypad lesen
uint16_t read_key(void){
	uint8_t low, high;
	if(!i2c_start(I2C_DISPLAY+I2C_WRITE)) {
		i2c_write(1);
		i2c_start(I2C_DISPLAY+I2C_READ);
		low = i2c_readAck();
		high = i2c_readNak();
		i2c_stop();
		return (((uint16_t)high) << 8) | low;
	}
	return 0xFFFF;
}

// Keypad testen
void lcd_test_key(void)
{
	display_output_string(1, 1, "123456789*0#");
	cursor_x_y(2, 1);
	for(uint8_t i=0; i<16; ++i)
		write_char(((read_key() & (1 << i)) >> i) + '0');
}
