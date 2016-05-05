// bno055.cpp: IMU-Klasse

////////////
// Header //
////////////

#include "bno055.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////
// Konstruktor
bno055::bno055(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
bno055::~bno055()
{
	//
}

// IMU initialisieren
void bno055::init(void)
{
	uint8_t buffer[2];
	buffer[0] = 0x3B; //Unit Selection Register
	//Einheiten 	Data Output Format (Android)|...|...|Temperatur (C)|...|Winkeleinheit (Grad)|Rotatationsgeschwindigkeit(Grad/sek)|Beschleunigung (m/s²)
	buffer[1] = (1<<7)|(0<<4)|(0<<2)|(0<<1)|(1<<0);
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
	
	buffer [0] = 0x3D; //Register 0x3D 
	//NDOF Modus auswählen (als letzte option da dann keine config mehr)
	#ifdef _FASTCALIB
		buffer[1] = 0b1100;
	#else
		buffer[1] = 0b1011;
	#endif
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
	
}

//Beschleunigungs Vektoren
int16_t bno055::accDataX(void)
{
	return this->getWord(0x8);
}

int16_t bno055::accDataY(void)
{
	return this->getWord(0xA);
}

int16_t bno055::accDataZ(void)
{
	return this->getWord(0xC);
}


//Magnetfeld Vektoren
int16_t bno055::magDataX(void)
{
	return this->getWord(0xE);
}

int16_t bno055::magDataY(void)
{
	return this->getWord(0x10);
}

int16_t bno055::magDataZ(void)
{
	return this->getWord(0x12);
}

//Gyro Vektoren
int16_t bno055::gyrDataX(void)
{
	return this->getWord(0x14);
}

int16_t bno055::gyrDataY(void)
{
	return this->getWord(0x16);
}

int16_t bno055::gyrDataZ(void)
{
	return this->getWord(0x18);
}

//Ausrichtung in Euler-Winkeln
int16_t bno055::eulHeading(void)
{
	return (int16_t)(this->getWord(0x1A)/16);
}

int16_t bno055::eulRoll(void)
{
	return (int16_t)(this->getWord(0x1C)/16);
}

int16_t bno055::eulPitch(void)
{
	return (int16_t)(this->getWord(0x1E)/16);
}

//Ausrichtung in Quaternionen
float bno055::quaDataW(void)
{
	return (int16_t)(this->getWord(0x20)/(1<<14));
}

float bno055::quaDataX(void)
{
	return (int16_t)(this->getWord(0x22)/(1<<14));
}

float bno055::quaDataY(void)
{
	return (int16_t)(this->getWord(0x24)/(1<<14));
}

float bno055::quaDataZ(void)
{
	return (int16_t)(this->getWord(0x26)/(1<<14));
}

//Lineare Beschleunigung Vektoren
float bno055::liaDataX(void)
{
	return this->getWord(0x28)/100;
}

float bno055::liaDataY(void)
{
	return this->getWord(0x2A)/100;
}	

float bno055::liaDataZ(void)
{
	return this->getWord(0x2C)/100;
}


//Gravitations Vektoren
float bno055::grvDataX(void)
{
	return this->getWord(0x2E)/100;
}

float bno055::grvDataY(void)
{
	return this->getWord(0x30)/100;
}

float bno055::grvDataZ(void)
{
	return this->getWord(0x32)/100;
}


//Temperatur
int8_t bno055::temp(void)
{
	uint8_t buffer[1] = { 0x34};
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


//Kalibrationsstatus
uint8_t bno055::calibStatus(void)
{
	uint8_t buffer[1] = { 0x35};
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

uint8_t bno055::selfTest(void){
	//Self Test starten
	uint8_t buffer[2] = {0x3F, (1<<0)};	
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);

	//Ergebnis lesen
	buffer[0] = 0x36;
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

void bno055::reset(void){
	//Self Test starten
	uint8_t buffer[2] = {0x3F, (1<<5)};
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

uint8_t bno055::systemError(void){
	uint8_t buffer[1] = { 0x3A};
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

uint8_t bno055::systemStatus(void){
	uint8_t buffer[1] = { 0x39};
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


//Private Funktion um zwei Bytes abzufragen
inline int16_t bno055::getWord(uint8_t startAddr){
	uint8_t buffer[1] = {startAddr};
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWriteRead(this->twi, this->addr, buffer, 1, 2);
	while(!::TWI_MasterReady(this->twi));
	if(this->twi->result == TWIM_RESULT_OK && this->twi->bytesRead == 2) {
		this->err = false;
		return (((int16_t)this->twi->readData[1]) << 8) | this->twi->readData[0];
	}
	else {
		this->err = true;
		return 0;
	}
}

