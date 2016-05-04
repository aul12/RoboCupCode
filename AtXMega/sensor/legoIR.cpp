// legoIR.cpp: Lego-IR-Sensor

////////////
// Header //
////////////

#include "legoIR.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
legoIR::legoIR(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
legoIR::~legoIR()
{
	//
}

// Auslesefunktion
uint8_t legoIR::dir(void)
{
	uint8_t IR_SCHWELLE = 130;
	uint8_t data[6];
	uint8_t buffer[1] = { 0x49 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 6);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 6) {
		for(uint8_t i=0; i<6; ++i)
			data[i] = this->twi->readData[i];
		this->err = false;
	}
	else {
		this->err = true;
		return -1;
	}
	buffer[0] = 0x42;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 6);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 6) {
		this->err = false;
		if(data[1] > IR_SCHWELLE || data[2] > IR_SCHWELLE || data[3] > IR_SCHWELLE || data[4] > IR_SCHWELLE || data[5] > IR_SCHWELLE)
			return this->twi->readData[0];
		else
			return data[0];
	}
	else {
		this->err = true;
		return -2;
	}
}
