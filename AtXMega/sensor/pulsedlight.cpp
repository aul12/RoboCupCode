////////////
// Header //
////////////

#include "pulsedlight.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
pulsedlight::pulsedlight(TWI_Master_t* twi, uint8_t addr)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
pulsedlight::~pulsedlight()
{
	//
}

void pulsedlight::init()
{
	uint8_t buffer[2];
	buffer[0] = 0x00; //Der zu schreibende Wert für die Initialisierung
	buffer[1] = 0x00;
	
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

uint16_t pulsedlight::distanceCM()
{
	uint8_t buffer[1] = { 0x8f };
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

void pulsedlight::startMeasurement(){
	uint8_t buffer[2];
	buffer[0] = 0x00; //Register 0x00
	buffer[1] = 0x04; //Der zu schreibende Wert für die Initialisierung
	
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

uint8_t pulsedlight::isBusy(){
	uint8_t buffer[1] = {0x01};	//Status Register
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 1);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 1) {
		this->err = false;
		return this->twi->readData[0] & (1<<8);
	}
	else {
		this->err = true;
		return 0;
	}
}