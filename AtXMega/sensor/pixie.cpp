// pixie.cpp: Kamera

////////////
// Header //
////////////

#include "pixie.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

//Konstruktor
Pixie::Pixie(TWI_Master_t* twi, uint8_t addr){
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

//Destruktor
Pixie::~Pixie(void){

}

//Datenabfrage via I²C
uint8_t Pixie::pollData(void){
	uint8_t counter = 0;
	while(this->getWord() != PIXY_START_WORD && ++counter<40){
		if(this->result == PIXIE_ERROR_TWI)
			return PIXIE_ERROR_TWI;
	}
	if(counter>=40){
		this->number = 0;
		this->x_pos = 0;
		this->y_pos = 0;
		this->width = 0;
		this->height = 0;
		this->result = PIXIE_ERROR_OBJECT;
		return PIXIE_ERROR_OBJECT;
	}
	
	if(this->getWord() == PIXY_START_WORD){
		this->checksum = getWord();
		this->number = getWord();
		this->x_pos = getWord();
		this->y_pos = getWord();
		this->width = getWord();
		this->height = getWord();
		#ifdef _CHECKSUM
			if(this->checksum != (this->number+this->x_pos+this->y_pos+this->width+this->height)){
				this->number = 0;
				this->x_pos = 0;
				this->y_pos = 0;
				this->width = 0;
				this->height = 0;
				this->result = PIXIE_ERROR_CHECKSUM;
				return PIXIE_ERROR_CHECKSUM;
			}
			else{
				this->result = PIXIE_RESULT_OK;
				return PIXIE_RESULT_OK;
			}
		#else
			this->result = PIXIE_RESULT_OK;
			return PIXIE_RESULT_OK;
		#endif
		
	}
	else
		return PIXIE_RESULT_OK;
}

uint16_t Pixie::getWidth(void){
	return this->width;
}

uint16_t Pixie::getHeight(void){
	return this->height;
}

uint16_t Pixie::getX(void){
	return this->x_pos;
}

uint16_t Pixie::getY(void){
	return this->y_pos;
}

uint16_t Pixie::getSignature(void){
	return this->number;
}

uint8_t Pixie::getResult(void){
	return this->result;
}


void Pixie::setBrightness(uint8_t brightness){
	uint8_t buffer[3] = {0x00, PIXY_CAM_BRIGHTNESS_SYNC, brightness };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 3);
}


void Pixie::ledColor(uint8_t r, uint8_t g, uint8_t b){
	uint8_t buffer[5] = {0x00, PIXY_LED_SYNC, r , g, b};
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 5);
}


uint16_t Pixie::getWord(void){
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, 0, 0, 2);
	while(!::TWI_MasterReady(this->twi));
	
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2){
		uint16_t w = twi->readData[1];
		uint16_t c = twi->readData[0];
		w <<= 8;
		w |= c;
		return w;
	}
	else{
		this->result = PIXIE_ERROR_TWI;
		return 0;
	}
}

