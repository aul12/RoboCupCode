// cmps03.hpp: C++-Klassenimplementation für I²C-Kompass

#ifndef _I2C_CMPS_IMP_
#define _I2C_CMPS_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error cmps03.hpp must be used from C++-file!
#endif

// Adresse
#define I2C_CMPS 0xC0

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

class cmps03 {
	public:
		// Konstruktor
		cmps03(TWI_Master_t* twi, uint8_t addr);
		~cmps03();
		
		// Auslesefunktionen
		uint8_t byte(void);
		int16_t grad(void);
		
		// Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };
		
	private:
		// verbotene Konstruktoren
		cmps03();
		
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;
	
};

#endif
