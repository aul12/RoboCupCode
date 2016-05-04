// bno055.hpp: C++-Klassenimplementation für I²C-IMU

#ifndef _I2C_IMU_IMP_
#define _I2C_IMU_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error bno055.hpp must be used from C++-file!
#endif

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
class bno055 {
	public:
		// Konstruktor
		bno055(TWI_Master_t* twi, uint8_t addr);
		~bno055();
		
		// Initialisieren
		void init(void);
		
		//Auslese Funktionen
		//Beschleunigungs Vektoren
		int16_t accDataX(void);
		int16_t accDataY(void);
		int16_t accDataZ(void);

		//Magnetfeld Vektoren
		int16_t magDataX(void);
		int16_t magDataY(void);
		int16_t magDataZ(void);

		//Gyro Vektoren
		int16_t gyrDataX(void);
		int16_t gyrDataY(void);
		int16_t gyrDataZ(void);

		//Ausrichtung in Euler-Winkeln
		int16_t eulHeading(void);
		int16_t eulRoll(void);
		int16_t eulPitch(void);

		//Ausrichtung in Quaternionen
		float quaDataW(void);
		float quaDataX(void);
		float quaDataY(void);
		float quaDataZ(void);

		//Lineare Beschleunigungs Vektoren
		float liaDataX(void);
		float liaDataY(void);
		float liaDataZ(void);

		//Gravitations Vektoren
		float grvDataX(void);
		float grvDataY(void);
		float grvDataZ(void);

		//Temperatur
		int8_t temp(void);

		//Kalibrationsstatus
		uint8_t calibStatus(void);

		//Selbsttest (Komponenten überprüfen)
		uint8_t selfTest(void);

		//IMU reset
		void reset(void);

		//Status (Fehler, starten...)
		uint8_t systemStatus(void);

		//Sensor Fehler (Fehler angabe)
		uint8_t systemError(void);
		

		// i2c Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };

	private:	
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;

		int16_t getWord(uint8_t startAddr);
	
};

#endif