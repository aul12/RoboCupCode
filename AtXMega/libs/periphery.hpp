// periphery.hpp: Peripherieger‰te

#ifndef _PERIPHERY_IMP_
#define _PERIPHERY_IMP_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error periphery.hpp must be used from C++-file!
#endif

////////////
// Header //
////////////

extern "C" {
	#include <stdint.h>
}


////////////////
// Namespaces //
////////////////

// Dribbler
namespace dribbler {
	// Steuerung
	void power(uint8_t ext);
};

// Schuss
namespace schuss {
	// Schieﬂen
	void Kick(void);
	void Kick_LP(void);
	
	// Update-Timer
	void tick(void);
};

// Lichtschranke
namespace ballda {
	// Variablen
	extern volatile bool ball_da;
	
	// Update-Timer
	void update(void);
	
	// Ballda?
	inline bool __attribute__ ((always_inline)) check(void)
	{ return ball_da; };
}

#endif
