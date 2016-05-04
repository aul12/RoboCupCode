// cmps03.cpp: Kompassklasse

////////////
// Header //
////////////

#include "cmps03.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
cmps03::cmps03(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
cmps03::~cmps03()
{
	//
}

// Kompasswert als Byte auslesen
uint8_t cmps03::byte(void)
{
	uint8_t buffer[1] = { 1 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 1);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 1) {
		this->err = false;
		return this->twi->readData[0];
	}
	else {
		this->err = true;
		return 0;
	}
}

// Kompasswert als 0-3600 auslesen
int16_t cmps03::grad()
{
	uint8_t buffer[1] = { 2 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		this->err = false;
		return (((int16_t)this->twi->readData[0]) << 8) | this->twi->readData[1];
	}
	else {
		this->err = true;
		return 0;
	}
}
