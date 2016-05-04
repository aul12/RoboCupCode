// lcd03.hpp: C++-Klassenimplementation für I²C-Display

#ifndef _I2C_DISPLAY_IMP_
#define _I2C_DISPLAY_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error lcd03.hpp must be used from C++-file!
#endif

// Adresse
#define I2C_DISPLAY 0xC6

// Keypad-Tasten
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

// Cursoreinstellungen
enum E_CURSOR {
	LCD_CURSOR_OFF = 0,
	LCD_CURSOR_UNDERLINE = 2,
	LCD_CURSOR_BLINK = 1
};

////////////
// Header //
////////////

extern "C" {
	#include <stdint.h>
	#include <stdlib.h>
	#include <string.h>
}
#include "../driver/twimaster.h"

///////////////////////////
// Klassenimplementation //
///////////////////////////

class lcd03 {
	public:
		// Konstruktor
		lcd03(TWI_Master_t* twi, uint8_t addr);
		~lcd03();
	
		// komplett Ausgabe
		void out_str(uint8_t zeile, uint8_t spalte, const char str[]);
		void out_num(uint8_t zeile, uint8_t spalte, int16_t zahl, int8_t base);
		void out_int(uint8_t zeile, uint8_t spalte, int16_t zahl);
		
		// Displaystatus
		void clear(void);
		void light(uint8_t on);
		void cursor(E_CURSOR cur);
		
		// einfache Ausgabe
		void set_cursor(uint8_t zeile, uint8_t spalte);
		void write_char(char c);
		void write_str(const char s[]);
		bool my_char(uint8_t num, char data[8]);
		
		// Keypad
		uint16_t read_key(void);
		void test_key(void);
		
	private:
		// verbotene Konstruktoren
		lcd03();
		
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
	
};

#endif
