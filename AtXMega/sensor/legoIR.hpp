// legoIR.hpp: C++-Klassenimplementation für I²C-Lego IR-Sensor

#ifndef _I2C_LEGO_IMP_
#define _I2C_LEGO_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error legoIR.hpp must be used from C++-file!
#endif

// Adresse
#define I2C_LEGOIR 0x10

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

class legoIR {
	public:
		// Konstruktor
		legoIR(TWI_Master_t* twi, uint8_t addr);
		~legoIR();
		
		// Auslesefunktionen
		uint8_t dir(void);
		
		// Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };
		
	private:
		// verbotene Konstruktoren
		legoIR();
		
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;
	
};

#endif
