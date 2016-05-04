//pulsedlight.hpp Klassenimplementation für i2c Pulsedlight-Entfernungssensor

#ifndef _PULSEDLIGHT_IMP
#define _PULSEDLIGHT_IMP
//Adresse
//#define PULSEDLIGHT_ADDRESS 0x62 //oder 0xC4???

#ifndef __cplusplus
#error bno055.hpp must be used from C++-file!
#endif

extern "C" {
	#include <stdint.h>
	#include <stdlib.h>
	
}
#include "../driver/twimaster.h"


//////////////////////////////////////////////////////////////////////////
///////////////////////////Klassenimplementation//////////////////////////
//////////////////////////////////////////////////////////////////////////

class pulsedlight {
	public:
		// Konstruktor
		pulsedlight(TWI_Master_t* twi, uint8_t addr);
		~pulsedlight();
		
		//Initialisierung
		void init(void);
		
		//Messung starten
		void startMeasurement();
		//Festestellen ob Sensor beschäftigt ist
		uint8_t isBusy();
		//Distanz in cm
		uint16_t distanceCM();
		
		
		// i2c Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };
		
	private:
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;
	};
	#endif