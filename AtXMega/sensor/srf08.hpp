// srf08.hpp: C++-Klassenimplementation für I²C-Kompass

#ifndef _I2C_US_IMP_
#define _I2C_US_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error srf08.hpp must be used from C++-file!
#endif

////////////
// Header //
////////////

extern "C" {
	#include <util/delay.h>
	#include <stdint.h>
}
#include "../driver/twimaster.h"

///////////////////////////
// Klassenimplementation //
///////////////////////////

class srf08 {
	public:
		// Konstruktor
		srf08(TWI_Master_t* twi, uint8_t addr);
		~srf08();
		
		// Auslesefunktionen
		void init(void);
		void init_start(void);
		void start(void);
		uint16_t read(void);
		uint16_t distance(void);
		bool ready(void);
		
		// Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };

	private:
		// verbotene Konstruktoren
		srf08();
		
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;
	
};

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Ultraschalladresse ändern
void change_srf08_addr(TWI_Master_t* twi, uint8_t old_addr, uint8_t new_addr);

#endif
