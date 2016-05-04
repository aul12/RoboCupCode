// atmega8.hpp: Linienbibliothek

#ifndef _ATMEGA8_IMP_
#define _ATMEGA8_IMP_

///////////////
// Variablen //
///////////////

// Liniensensoren
extern volatile uint8_t linie[32];		// Linienwerte (0 -> gr�n, 1 -> wei�)
extern volatile int16_t out_winkel;		// Aus-Winkel (relativ zum Roboter, wie Fahrtrichtung)
extern volatile uint8_t out;			// Au�erhalb des Feldes Zustand (0 -> innerhalb, 1 -> vielleicht, 2 -> au�erhalb, 3 -> kritisch, 4 -> zu sp�t)
extern volatile uint8_t line_regler;	// Linienregler (Logik 3&4)
extern volatile uint8_t out_pos;
extern int16_t mWinkel;		//korrigierter Messwinkel
extern int8_t preRichtung; //erste Linienrichtung
extern int8_t lRichtung; //Linienrichtung
extern bool lMuesli;		//#ReturnOfZeM�sli
extern int8_t lEcke;		//aktuelle Ecke (return*90+45)
extern volatile uint8_t trans;

// Timer Failsave
extern volatile uint8_t timer_fail;		// Z�hlvariable f�r Timer-Fail

////////////////
// Funktionen //
////////////////

// Kommunikation mit Atmega8 initialisieren
extern void atmega8_init(void);

// Kalibration von CoBoard
extern void atmega8_kalibration(uint8_t enable);

#endif
