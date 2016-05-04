// periphery.cpp: Peripherieger�te

//PDI BELEGUNG
/*
	 ____
	|3	4|
	 X	X|
	|6	2|
	 ----
*/

////////////
// Header //
////////////

extern "C" {
	#include <avr/io.h>		// I/O - Ports
	#include <math.h>		// Trigonometrische Funktionen
}
#include "../defines.h"		// globale Definitionen
#include "motor.hpp"		// Motorfunktionen
#include "periphery.hpp"

///////////////
// Variablen //
///////////////

// Dribbler
namespace dribbler {
	volatile bool force_off = false;		// Zwangsausschalten
	volatile uint8_t nachlauf = 1;			// Dribbler anlassen
};

// Schuss
namespace schuss {
	volatile bool schuss_OK = true;			// Schuss darf benutzt werden
	volatile uint16_t schuss_counter = 0;	// Counter f�r Schuss-BETRAGt�nde
};

// Lichtschranke
namespace ballda {
	volatile bool ball_da = false;			// Ball in Lichtschranke
	volatile uint16_t ball_t = 0;			// Fehlercounter
};

////////////////////////
// Dribblerfunktionen //
////////////////////////

// Dribblersteuerung
void dribbler::power(bool ext)
{
	if(!force_off && (ext || ::ballda::ball_da)) {
		//Dribbler an
		TCC0.CCA = 1000;
		nachlauf = 7;
	}
	else {
		if(--nachlauf == 0) {
			//Dribbler aus
			TCC0.CCA = 500;
			nachlauf = 1;
		}
	}
}

//////////////////////
// Schussfunktionen //
//////////////////////

// Schie�en
extern volatile uint16_t ball_counter;
void schuss::Kick(void)
{
	#ifdef _KICK
		if(schuss_OK) {
			::dribbler::force_off = true;
			Fahrtrichtung(0, SPEED_WEIT);
			wdt_delay(100);
			PORTK.OUTSET = (1 << 2);
			wdt_delay(KICK_DAUER);
			PORTK.OUTCLR = (1 << 2);
			::ballda::ball_t = 0;
			::ball_counter = -2;
			schuss_OK = false;
			::dribbler::force_off = false;
		}
	#endif
}

// Schie�en (wenig Kraft)
void schuss::Kick_LP(void)
{
	#ifdef _KICK
		if(schuss_OK) {
			PORTK.OUTSET = (1 << 2);
			wdt_delay(KICK_DAUER_LOW);
			PORTK.OUTCLR = (1 << 2);
			::ballda::ball_t = 0;
			schuss_OK = false;
		}
	#endif
}

// Schusstimer
void schuss::tick(void)
{
	if(!schuss_OK && ++schuss_counter > SCHUSS_DELAY) {
		schuss_counter = 0;
		schuss_OK = true;
	}
}

///////////////////
// Lichtschranke //
///////////////////

// �berpr�fen
void ballda::update(void)
{
    if(LICHTSCHRANKE) {
		if(ball_t > BALLDA_SCHWELLE) {
			ball_da = true;
			SETLED(4);
			if(ball_t < BALLDA_SCHWELLE_MAX)
				++ball_t;
		}
		else {
			ball_da = false;
			CLEARLED(4);
			++ball_t;
		}
	}
	else {
		if(ball_t > BALLDA_SCHWELLE) {
			ball_da = true;
			SETLED(4);
			--ball_t;
		}
		else {
			ball_da = false;
			CLEARLED(4);
			if(ball_t > 0)
				--ball_t;
		}
	}
}
