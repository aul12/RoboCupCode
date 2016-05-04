// ltc2945.cpp: Spannungsmesser-Klasse

////////////
// Header //
////////////
#include "ltc2945.hpp"

// Konstruktor
ltc2945::ltc2945(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
ltc2945::~ltc2945()
{
	
}


uint16_t ltc2945::getVoltage()
{
	uint8_t buffer[1] = {0x1E};
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		this->err = false;
		uint16_t wert = (((int16_t)this->twi->readData[0]) << 8) | this->twi->readData[1];
		return wert*8/5;
	}
	else {
		this->err = true;
		return 0;
	}
}

uint16_t ltc2945::getCurrent()
{
	uint8_t buffer[1] = {0x14};
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		this->err = false;
		uint16_t wert = (((int16_t)this->twi->readData[0]) << 8) | this->twi->readData[1];
		return wert*4/13;
	}
	else {
		this->err = true;
		return 0;
	}
}
