// ir.hpp: Funktionen zum Auslesen der Infrarotsensoren

#ifndef _ADC_INC_
#define _ADC_INC_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error ir.hpp must be used from C++-file!
#endif

//////////////////////
// Inlinefunktionen //
//////////////////////

// Balldistanz korrigieren
#ifdef _MAIN_FILE_
inline uint16_t __attribute__ ((always_inline)) corrDistanz(uint16_t dist, int16_t winkel)
{
	// Korrektur gegen Räder
	return (uint16_t)Round(dist * (-cosinus(4*winkel)+9)/8.0f);
}
#endif

////////////////////////
// Funktionsanmeldung //
////////////////////////

// ADC Interrupt initialisieren
void ADC_init(void);
void ADC_initB(void);

#endif
