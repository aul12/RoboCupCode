// motor.cpp: Funktionen zur Motoransteuerung

////////////
// Header //
////////////

extern "C" {
	#include <avr/io.h>			// I/O - Ports
	#include <avr/wdt.h>		// Watchdog / automatischer Reset
	#include <util/delay.h>		// Delay Cycles
	#include <math.h>			// Trigonometrische Funktionen
}
#include "../defines.h"			// Roboterdefinitionen
#include "sinus.h"				// Sinusfunktion
#include "atmega8.hpp"			// Linienfunktionen
#include "motor.hpp"


///////////////////////
// externe Variablen //
///////////////////////

extern volatile int16_t k[4];
extern volatile uint8_t Soft_Counter;
extern volatile int16_t US_pos[2];
extern volatile uint8_t dribblerInit;
extern volatile int16_t rpmHall;
extern volatile int16_t hallPower;
extern volatile int16_t hallFlaeche;
extern volatile int16_t lastDelta;

////////////////
// Funktionen //
////////////////

// Motorgeschwindigkeit setzen
void MotorPower(uint8_t motor_id, int16_t power)
{
	// Power-Wert zwischen +-MAX_PWM, Vorzeichen gibt die Richtung an
	uint16_t p;
	p = BETRAG(power);
	if(p > MAX_PWM)
		p = MAX_PWM;
	if(p < MIN_PWM && p > 200)
		p = MIN_PWM;
		
	// Motorsteuerung
	if(power > 0) {
		PORTK.OUTSET = 0b10000 << (motor_id);
		
		p = MAX_PWM - p;
		
	}
	else {
		PORTK.OUTCLR = 0b10000 << (motor_id);
		
	}
		
	// Ports schalten
	if(motor_id == 0)
		TCE0.CCA = p;
	else if(motor_id == 1)
		TCE0.CCB = p;
	else if(motor_id == 2)
		TCE0.CCC = p;
	else
		TCE0.CCD = p;

	
	
}

/*void MotorRotation(uint8_t motor_id, int16_t rotation)
{
	int16_t delta = rotation - rpmHall[motor_id];
	
	if(delta * lastDelta[motor_id] <= 0)
		hallFlaeche[motor_id] = 0;
	else
		hallFlaeche[motor_id] += delta;
		
	
	hallPower[motor_id] += delta * HALL_P + hallFlaeche[motor_id] * HALL_I + (delta-lastDelta[motor_id]) * HALL_D;
	
	if(hallPower[motor_id] < -MAX_PWM)
		hallPower[motor_id] = MAX_PWM;
	else if(hallPower[motor_id] > MAX_PWM)
		hallPower[motor_id] = MAX_PWM;
	MotorPower(motor_id, hallPower[motor_id]);
}*/

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung(int16_t alpha, float speed)
{
	// Arrays mit Motorleistung für 0° und 90°
	const float k0[4] = { 1, 1, -1, -1 };
	const float k90[4] = { 1, -1, -1, 1 };
	// Arrays für Motorleistung
	float kx[4];
	float ky[4];

	// Werte anhand des Winkels errechnen
	for(uint8_t i=0; i<4; i++) {
		// Einzelvektoren berechnen
		kx[i] = sinus(alpha) * k90[i] * speed;
		ky[i] = cosinus(alpha) * k0[i] * speed;
		// Vektoren zusammenrechnen
		k[i] = (int16_t)Round(kx[i] + ky[i]);
	}
}
void FahrtrichtungB(int16_t alpha, float speed)
{
	// Arrays mit Motorleistung für 0° und 90°
	const float k0[4] = { 1, 1, -1, -1 };
	const float k90[4] = { 1, -1, -1, 1 };
	// Arrays für Motorleistung
	float kx[4];
	float ky[4];
	
	// Winkel anpassen
	while(alpha > 180)
		alpha -= 360;
	while(alpha < -180)
		alpha += 360;

	// Werte anhand des Winkels errechnen
	for(uint8_t i=0; i<4; i++) {
		// Einzelvektoren berechnen
		#if _COMPLEX_LINE==1
			if(out >= 2) {
				int16_t diff =  out_winkel-alpha;
				if(diff > 180)
					diff -= 360;
				else if(diff < -180)
					diff += 360;
				if(BETRAG(diff) < 90) {
					alpha = (diff>0) ? (out_winkel-90) : (out_winkel+90);
					speed *= cosinus(BETRAG(diff));
				}
			}
		#elif _COMPLEX_LINE==3
			if(out >= 1) {
				int16_t diff =  (-out_winkel-180)-alpha;
				while(diff > 180)
					diff -= 360;
				while(diff < -180)
					diff += 360;
				if(BETRAG(diff) < 90) {
					alpha = (diff>0) ? (-out_winkel-270) : (-out_winkel-90);
					speed *= cosinus(BETRAG(diff));
				}
			}
		#elif _COMPLEX_LINE==5
			if(out >= 2) {
				int16_t diff =  out_winkel-alpha;
				if(diff > 180)
					diff -= 360;
				else if(diff < -180)
					diff += 360;
				if(BETRAG(diff) < 90) {
					alpha = (diff>0) ? (out_winkel-90) : (out_winkel+90);
					speed *= cosinus(BETRAG(diff));
				}
				if(speed>SPEED_MAX_LINIE)
					speed=SPEED_MAX_LINIE;
				else if(speed<-SPEED_MAX_LINIE)
					speed= SPEED_MAX_LINIE;
			}
		#elif _COMPLEX_LINE==6
			if(out >= 1) {
				int16_t diff =  (-out_winkel-180)-alpha;
				while(diff > 180)
					diff -= 360;
				while(diff < -180)
					diff += 360;
				if(BETRAG(diff) < 90) {
					alpha = (diff>0) ? (-out_winkel-270) : (-out_winkel-90);
					speed *= cosinus(BETRAG(diff));
				}
				if(speed>SPEED_MAX_LINIE)
					speed=SPEED_MAX_LINIE;
				else if(speed<-SPEED_MAX_LINIE)
					speed= SPEED_MAX_LINIE;
			}

		#endif
		kx[i] = sinus(alpha) * k90[i] * speed;
		ky[i] = cosinus(alpha) * k0[i] * speed;

		// Vektoren zusammenrechnen
		k[i] = (int16_t)Round(kx[i] + ky[i]);
		
		#if _COMPLEX_LINE==8
			if(out>0)
				k[i] /= 2;
		#endif
	}
}

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung_XY(int16_t X, int16_t Y)
{
	#ifdef _US
		#ifdef _US_PID
			// Zählvariable
			static uint8_t dt_counter = 0;
			static float Integral_X = 0;
			static float Integral_Y = 0;
			static int16_t delta_X_alt = 61;
			static int16_t delta_Y_alt = 22;
		#endif

		// Arrays mit Motorleistung für 0° und 90°
		const float k0[4] = { -1, -1, 1, 1 };
		const float k90[4] = { 1, -1, -1, 1 };
		// Arrays für Motorleistung
		float kx[4];
		float ky[4];
		// Positionen
		int16_t delta_X = US_pos[0] - X;
		int16_t delta_Y = US_pos[1] - Y;
		float diff_X = 0;
		float diff_Y = 0;
	
		#ifdef _US_PID
			// I & D ausrechnen
			if(dt_counter != Soft_Counter) {
				Integral_X += dt/5.0 * (US_pos[0] - X);
				if(Integral_X > usPID_flaeche_Max)
					Integral_X = usPID_flaeche_Max;
				else if(Integral_X < -usPID_flaeche_Max)
					Integral_X = -usPID_flaeche_Max;
				if(((float)(US_pos[0]-X))*Integral_X <= 0)
					Integral_X = 0;
				
				Integral_Y += dt/5.0 * (US_pos[1] - Y);
				if(Integral_Y > usPID_flaeche_Max)
					Integral_Y = usPID_flaeche_Max;
				else if(Integral_Y < -usPID_flaeche_Max)
					Integral_Y = -usPID_flaeche_Max;
				if(((float)(US_pos[1]-Y))*Integral_Y <= 0)
					Integral_Y = 0;
				
				diff_X = (delta_X - delta_X_alt)/dt*5.0;
				diff_Y = (delta_Y - delta_Y_alt)/dt*5.0;
				
				delta_X_alt = delta_X;
				delta_Y_alt = delta_Y;
				dt_counter = Soft_Counter;
			}
		#endif

		// Werte anhand des Winkels errechnen
		for(uint8_t i=0; i<4; i++) {
			// Einzelvektoren berechnen
			#ifdef _US_PID
				kx[i] = (delta_X * US_P + Integral_X * US_I + diff_X * US_D) * k90[i];
				ky[i] = (delta_Y * US_P + Integral_Y * US_I + diff_Y * US_D) * k0[i];
			#else
				kx[i] = delta_X * US_P * k90[i];
				ky[i] = delta_Y * US_P * k0[i];
			#endif
			// Vektoren zusammenrechnen
			k[i] = (int16_t)Round(kx[i] + ky[i]);
		}

		// Optimierungsalgorithmus
		int16_t max_power = 0;
		uint8_t i;
	
		for(i=0; i<4; i++) {
			if(BETRAG(k[i]) > max_power) 
			max_power = BETRAG(k[i]);
		}
		if(max_power > US_MAX_POWER) {
			for(i=0; i<4; i++) {
				k[i] = (int16_t)Round(k[i]*(US_MAX_POWER/(float)max_power));
			}
		}
	#else
		k[0] = k[1] = k[2] = k[3] = 0;
	#endif
}

// Einstellen der Roboter-Fahrtrichtung
void Fahrtrichtung_Y(int16_t X_speed, int16_t Y)
{
	#ifdef _US
		#ifdef _US_PID
			// Zählvariable
			static uint8_t dt_counter = 0;
			static float Integral_Y = 0;
			static int16_t delta_Y_alt = 22;
		#endif

		// Arrays mit Motorleistung für 0° und 90°
		const float k0[4] = { -1, -1, 1, 1 };
		const float k90[4] = { 1, -1, -1, 1 };
		// Arrays für Motorleistung
		float kx[4];
		float ky[4];
		// Positionen
		int16_t delta_Y = US_pos[1] - Y;
		float diff_Y = 0;
	
		#ifdef _US_PID
			// I & D ausrechnen
			if(dt_counter != Soft_Counter) {
				Integral_Y += dt/5.0 * (US_pos[1] - Y);
				if(Integral_Y > usPID_flaeche_Max)
					Integral_Y = usPID_flaeche_Max;
				else if(Integral_Y < -usPID_flaeche_Max)
					Integral_Y = -usPID_flaeche_Max;
				if(((float)(US_pos[1]-Y))*Integral_Y <= 0)
					Integral_Y = 0;
				
				diff_Y = (delta_Y - delta_Y_alt)/dt*5.0;
				
				delta_Y_alt = delta_Y;
				dt_counter = Soft_Counter;
			}
		#endif

		// Werte anhand des Winkels errechnen
		for(uint8_t i=0; i<4; i++) {
			// Einzelvektoren berechnen
			#ifdef _US_PID
				kx[i] = k90[i] * X_speed;
				ky[i] = (delta_Y * US_P + Integral_Y * US_I + diff_Y * US_D) * k0[i];
			#else
				kx[i] = k90[i] * X_speed;
				ky[i] = delta_Y * US_P * k0[i];
			#endif
			// Vektoren zusammenrechnen
			k[i] = (int16_t)Round(kx[i] + ky[i]);
		}

		// Optimierungsalgorithmus
		int16_t max_power = 0;
		uint8_t i;
	
		for(i=0; i<4; i++) {
			if(BETRAG(k[i]) > max_power) 
			max_power = BETRAG(k[i]);
		}
		if(max_power > MAX_PWM) {
			for(i=0; i<4; i++) {
				k[i] = (int16_t)Round(k[i]*(MAX_PWM/(float)max_power));
			}
		}
		
	#else
		// Werte anhand des Winkels errechnen
		const float k90[4] = { 1, -1, -1, 1 };
		for(uint8_t i=0; i<4; i++) {
			// Vektoren zusammenrechnen
			k[i] = (int16_t)Round(k90[i] * X_speed);
		}

		// Optimierungsalgorithmus
		int16_t max_power = 0;
		uint8_t i;
	
		for(i=0; i<4; i++) {
			if(BETRAG(k[i]) > max_power) 
			max_power = BETRAG(k[i]);
		}
		if(max_power > MAX_PWM) {
			for(i=0; i<4; i++) {
				k[i] = (int16_t)Round(k[i]*(MAX_PWM/(float)max_power));
			}
		}
	#endif
}

// Delay-Funktion
void wdt_delay(uint16_t delay)
{
	// Watchdog zurücksetzen
	wdt_reset();
	
	// Abarbeiten und Watchdog zurücksetzen
	while(delay > 6) {
		_delay_ms(5);
		delay -= 6;
		wdt_reset();
	}
	
	// Letzte Verzögerung
	_delay_ms(delay);
	
	// Watchdog zurücksetzen
	wdt_reset();
}