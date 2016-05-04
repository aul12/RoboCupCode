// lcd03.cpp: Displayklasse

////////////
// Header //
////////////

#include "lcd03.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
lcd03::lcd03(TWI_Master_t* twi, uint8_t addr)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
lcd03::~lcd03()
{
	//
}

// String ausgeben
void lcd03::out_str(uint8_t zeile, uint8_t spalte, const char str[])
{
	uint8_t buffer[25] = { 0 };
	buffer[1] = 3;
	buffer[2] = zeile;
	buffer[3] = spalte;
	uint8_t size = ::strlen(str);
	if(size > 20)
		size = 20;
	::memcpy(buffer+4, str, size);
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, size+4);
}

// Integer zur Basis "base" ausgeben
void lcd03::out_num(uint8_t zeile, uint8_t spalte, int16_t zahl, int8_t base)
{
	char text[20] = { 0 };
	::itoa(zahl, text, base);
	this->out_str(zeile, spalte, text);
}

// Integer mit Basis 10 ausgeben
void lcd03::out_int(uint8_t zeile, uint8_t spalte, int16_t zahl)
{
	char text[6] = { 0 };
	::itoa(zahl, text, 10);
	this->out_str(zeile, spalte, text);
}

// Display leeren
void lcd03::clear(void)
{
	uint8_t buffer[2] = { 0, 12 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Beleuchtung
void lcd03::light(uint8_t on)
{
	uint8_t buffer[2] = { 0 };
	buffer[1] = on ? 19 : 20;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Cursor anzeigen
void lcd03::cursor(E_CURSOR cur)
{
	uint8_t buffer[2] = { 0 };
	if(cur == 0)
		buffer[1] = 4;
	else if(cur == 2)
		buffer[1] = 5;
	else
		buffer[1] = 6;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Cursorposition ändern
void lcd03::set_cursor(uint8_t zeile, uint8_t spalte)
{
	uint8_t buffer[4] = { 0, 3, 0, 0 };
	buffer[2] = zeile;
	buffer[3] = spalte;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 4);
}

// Zeichen schreiben
void lcd03::write_char(char c)
{
	uint8_t buffer[2] = { 0 };
	buffer[1] = c;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Zeichenkette schreiben
void lcd03::write_str(const char s[])
{
	uint8_t buffer[22] = { 0 };
	uint8_t size = ::strlen(s);
	if(size > 20)
		size = 20;
	::memcpy(buffer, s, size);
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, size+1);
}

// eigenes Zeichen (num 128-135)
bool lcd03::my_char(uint8_t num, char data[8])
{
	// Grenzen abchecken
	if(num < 128 || num > 135)
		return false;
		
	// Befehl senden
	uint8_t buffer[11];
	buffer[0] = 0;
	buffer[1] = 27;
	buffer[2] = num;
	::memcpy(buffer+3, data, 8);
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 11);
	return true;
}

// Zeichen von Keypad lesen
uint16_t lcd03::read_key(void)
{
	uint8_t buffer[1] = { 1 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		return ((this->twi->readData[0]) | (((uint16_t)this->twi->readData[1]) << 8));
	}
	else {
		return -1;
	}
}

// Keypad testen
void lcd03::test_key(void)
{
	this->out_str(1, 1, "123456789*0#");
	this->set_cursor(2, 1);
	for(uint8_t i=0; i<16; ++i)
		this->write_char(((this->read_key() & (1<<i)) >> i) + '0');
}
