// motor.hpp: Funktionen zur Motoransteuerung

#ifndef _MOTOR_INC_
#define _MOTOR_INC_

//////////////////
// Definitionen //
//////////////////

#ifndef __cplusplus
#error motor.hpp must be used from C++-file!
#endif

//////////////////////
// Inlinefunktionen //
//////////////////////

// Runden
inline float __attribute__ ((always_inline)) Round(float r)
{
	return ((r>0.0) ? floor(r+0.5) : ceil(r-0.5));
}

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Motorgeschwindigkeit setzen
void MotorPower(uint8_t motor_id, int16_t power);
void MotorRotation(uint8_t motor_id, int16_t rotation);

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung(int16_t alpha, float speed);
void FahrtrichtungB(int16_t alpha, float speed);

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung_XY(int16_t X, int16_t Y);

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung_Y(int16_t X_speed, int16_t Y);

// Delay-Funktion
void wdt_delay(uint16_t delay);

#endif
