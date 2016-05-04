// ltc2945.hpp: C++-Klassenimplementation für I²C-Spannugsmesser


#ifndef __LTC2945_H__
#define __LTC2945_H__


//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error ltc2945.hpp must be used from C++-file!
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

class ltc2945
{
	public:
		ltc2945(TWI_Master_t* twi, uint8_t addr);
		~ltc2945();
		uint16_t getVoltage();
		uint16_t getCurrent();
		
		// i2c Fehler
		inline bool __attribute__ ((always_inline)) error(void)
		{ return this->err; };

	private:
		// Variablen
		TWI_Master_t* twi;
		uint8_t addr;
		bool err;


}; //ltc2945

#endif //__LTC2945_H__
