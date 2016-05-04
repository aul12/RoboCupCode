// srf08.cpp: Ultraschallklasse

////////////
// Header //
////////////

#include "srf08.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
srf08::srf08(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
srf08::~srf08()
{
	//
}

// Ultraschallsensor initialisieren
void srf08::init(void)
{
	uint8_t buffer[3] = { 1, 0, 48 }; // Registerwahl | Gain | Distanz (2m -> 43mm x 48 -> ca. 16ms)
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 3);
}

// Neue Messung starten (mit Initialisierung)
void srf08::init_start(void)
{
	this->init();
	this->start();
}

// Neue Messung starten (ohne Initialisierung)
void srf08::start(void)
{
	uint8_t buffer[2] = { 0, 0x51 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Messung lesen
uint16_t srf08::read(void)
{
	uint8_t buffer[1] = { 2 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		this->err = false;
		return (((uint16_t)this->twi->readData[0]) << 8) | this->twi->readData[1];
	}
	else {
		this->err = true;
		return 0;
	}
}

// Komplette Messung
uint16_t srf08::distance(void)
{  
	this->start();
	while(!this->ready());
	return this->read();
}

// Messung beendet
bool srf08::ready(void)
{
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteNull(this->twi, this->addr);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK) {
		return true;
	}
	else {
		return false;
	}
}

////////////////
// Funktionen //
////////////////

// Adresse für Ultraschallsensor ändern
void change_srf08_addr(TWI_Master_t* twi, uint8_t old_addr, uint8_t new_addr)
{
	uint8_t buffer[2] = { 0, 0xA0 };
	while(!::TWI_MasterReady(twi));
	::TWI_MasterWrite(twi, old_addr, buffer, 2);
	_delay_ms(50);
	buffer[1] = 0xAA;
	while(!::TWI_MasterReady(twi));
	::TWI_MasterWrite(twi, old_addr, buffer, 2);
	_delay_ms(50);
	buffer[1] = 0xA5;
	while(!::TWI_MasterReady(twi));
	::TWI_MasterWrite(twi, old_addr, buffer, 2);
	_delay_ms(50);
	buffer[1] = new_addr;
	while(!::TWI_MasterReady(twi));
	::TWI_MasterWrite(twi, old_addr, buffer, 2);
}
