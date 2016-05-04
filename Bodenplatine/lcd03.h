// lcd03.h: Displaybibliothek

#ifndef _I2C_DISPLAY_IMP_
#define _I2C_DISPLAY_IMP_

//////////////////
// Definitionen //
//////////////////

// Adresse
#define I2C_DISPLAY 0xC6

// Keypad
#define LCD_KEY_1 0
#define LCD_KEY_2 1
#define LCD_KEY_3 2
#define LCD_KEY_4 3
#define LCD_KEY_5 4
#define LCD_KEY_6 5
#define LCD_KEY_7 6
#define LCD_KEY_8 7
#define LCD_KEY_9 8
#define LCD_KEY_ST 9
#define LCD_KEY_0 10
#define LCD_KEY_RT 11

// Cursor
enum E_CURSOR {
	LCD_CURSOR_OFF = 0, // Cursor off
	LCD_CURSOR_BLINK = 1, // Cursor blinking
	LCD_CURSOR_UNDERLINE = 2 // Curser underline
};

////////////
// Header //
////////////

#include <stdint.h>

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Einfacher Output
void display_output_string(uint8_t zeile, uint8_t spalte, char text[]);
void display_output_num(uint8_t zeile, uint8_t spalte, uint16_t zahl, uint8_t num);
void display_output_int(uint8_t zeile, uint8_t spalte, uint16_t zahl);

// Verwaltung
void clear_lcd(void);
void cursor_x_y(uint8_t zeile, uint8_t spalte);
void cursor_sichtbar(enum E_CURSOR cur);
void lcd_light(uint8_t on);

// Einfaches Senden
void write_char(unsigned char c);
void lprint(const char *s);
void lcd_my_char(char num, char data[8]);

// Keypad
uint16_t read_key(void);
void lcd_test_key(void);

#endif
