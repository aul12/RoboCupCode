// mpu6050.hpp: C++-Klassenimplementation für I²C-Accelerometer

#ifndef _I2C_MPU_IMP_
#define _I2C_MPU_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error mpu6050.hpp must be used from C++-file!
#endif

// Adresse
#define I2C_MPU 0xD0

////////////
// Header //
////////////

extern "C" {
	#include <stdint.h>
}
#include "../driver/twimaster.h"

///////////////////////////
// Klassenimplementation //
///////////////////////////

class mpu6050 {
	public:
		// Konstruktor
		mpu6050(TWI_Master_t* twi, uint8_t addr);
		~mpu6050();
		
		// Initialisierung
		void init(void);
		void reset(void);
		
		// Auslesefunktionen
		int16_t yaw(void);
		int16_t pitch(void);
		int16_t roll(void);
		int16_t acc_x(void);
		int16_t acc_y(void);
		int16_t acc_z(void);
		uint16_t temp(void);
		
		// Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };
		
	private:
		// verbotene Konstruktoren
		mpu6050();
		
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;
	
};

#endif
