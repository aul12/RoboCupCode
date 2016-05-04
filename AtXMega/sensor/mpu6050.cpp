// mpu6050.cpp: Beschleunigungsklasse

////////////
// Header //
////////////

extern "C" {
	#include <util/delay.h>	// Delay Cycles
}
#include "mpu6050.hpp"

///////////////////////
// Klassenfunktionen //
///////////////////////

// Konstruktor
mpu6050::mpu6050(TWI_Master_t* twi, uint8_t addr) : err(false)
{
	// Variablen speichern
	this->twi = twi;
	this->addr = addr;
}

// Destruktor
mpu6050::~mpu6050()
{
	//
}

// Initialisieren
void mpu6050::init(void)
{
	// Reset MPU
	uint8_t buffer[4] = { 0x6B, 0x80, 0x18, 0x00 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
	
	// Warten
	_delay_ms(20);
	
	// Wake MPU
	buffer[1] = 0;
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
	
	// Config MPU
	buffer[0] = 0x1A;
	buffer[1] = 0x06; // Tiefpass 0 (schnell) bis 6 (langsam)
	// buffer[2] -> Gyro, Test, Range tttrr--- 0 -> 250°/s, 3 -> 2000°/s
	// buffer[3] -> Beschleunigungssensor tttrr--- 0 -> 2g, 3 -> 16g
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 4);
}

// Reset
void mpu6050::reset(void)
{
	uint8_t buffer[2] = { 0x68, 0x04 };
	while(!::TWI_MasterReady(this->twi));
	::TWI_MasterWrite(this->twi, this->addr, buffer, 2);
}

// Yaw Messung
int16_t mpu6050::yaw(void)
{
	uint8_t buffer[1] = { 0x43 };
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

// Pitch Messung
int16_t mpu6050::pitch(void)
{
	uint8_t buffer[1] = { 0x45 };
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

// Roll Messung
int16_t mpu6050::roll(void)
{
	uint8_t buffer[1] = { 0x47 };
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

// Temperatur Messung
uint16_t mpu6050::temp()
{
	uint8_t buffer[1] = { 0x41 };
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

// Beschleunigungssensor X-Richtung
int16_t mpu6050::acc_x(void)
{
	uint8_t buffer[1] = { 0x3D };
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

// Beschleunigungssensor Y-Richtung
int16_t mpu6050::acc_y(void)
{
	uint8_t buffer[1] = { 0x3B };
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

// Beschleunigungssensor Z-Richtung
int16_t mpu6050::acc_z(void)
{
	uint8_t buffer[1] = { 0x3F };
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
