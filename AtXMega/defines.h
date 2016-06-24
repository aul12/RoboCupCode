// defines.h: Microcontroller-Spezifische Definitionen

#ifndef _STDDEFS_INC_
#define _STDDEFS_INC_

//////////////////
// Definitionen //
//////////////////

// Welcher Roboter (DIP-Schalter unter Breakout)
#define ROBO (DIP(1)?1:0)

//Ultraschall benutzen
#define _US

//IMU benutzen  --> BoschSensor
#define _IMU
#define _FASTCALIB	//Schnelle Kalibration, eventuell größere Störungen
#define _GYRO_ONLY //Werte nur mit Gyro-Integration
//#define _CMPS

// Sharpsensor
#define _TSOP

//Pixie
//#define _PIXIE

//Bluetooth
#define BLUETOOTH_ENABLE false

//Anti-Kipp Regelung
#define _ESP

//Spannungmessung
#define _POWER_MEASURE

//Mit Dribbler fahren (because Hardware...)
//#define _DRIBBLER

//Display-Debug
#define DISPLAY_COUNTER_WDH 50
#define PAGE_COUNT 25

// Schuss benutzen
#define _SCHUSS // Schuss in Spielfunktion
#define _KICK // Schuss in Kickfunktion
#define KICK_DAUER 100
#define SCHUSS_DELAY 254

// Spielart -> standard Soccer B
//#define SUPERFIELD

// Linienerkennung
// _COMPLEX_LINE = 0 => einfache Linienerkennung
// _COMPLEX_LINE = 1 => Lukas Logik -> FahrtrichtungB
// _COMPLEX_LINE = 2 => Lukas Logik -> sofortiges Dagegensteuern
// _COMPLEX_LINE = 3 => Hannes Logik -> FahrtrichtungB
// _COMPLEX_LINE = 4 => Hannes Logik -> sofortiges Dagegensteuern
// _COMPLEX_LINE = 5 => Lukas Logik -> Langsam auf Linie (nicht getestet)
// _COMPLEX_LINE = 6 => Hannes Logik -> Langsam auf Linie (nicht getestet)
// _COMPLEX_LINE = 7 => Linienerkennung über US (nur wenn Linie kaputt!)
// _COMPLEX_LINE = 8 => Tim Logik (funktioniert wahrscheinlich als einzige)
#define _COMPLEX_LINE 8

// Position
// US_n_pos => normale Position
// US_tw_pos => korrigierte Position
#define US_pos US_tw_pos

// Linie
#define DELAY_LINIE 300

// PID-Werte
#define PID_Counter_Max 4
#define PID_flaeche_Max 200

#define BALL_P 0.6
#define BALL_D 0.2
#define MAX_DREH_BALL 1000

#define PID_P 15.625
#define PID_I 8.0
#define PID_D 0.0625

#define US_P 12.0
#define US_I 5.0
#define US_D 0.4
#define MPU_ROLL 1
// Ballanfahrt
#define SPEED_LINIE 1500 // Linie kritisch
#define SPEED_WEIT 1000// Ball weit weg -> gerade Anfahrt
#define SPEED_KREIS 1200// Ball hinter Roboter -> Kreis
#define SPEED_NAH 750 // Ball vor Roboter -> Halbkreis
#define SPEED_SEITE 700 // seitlich zum Tor fahren
#define SPEED_TORWART 1200 // Torwart 2 X
#define SPEED_BALL 1300 //Geschwindigkeit mit Ball
#define US_MAX_POWER 1200 // Positionsfahrt
#define SPEED_SUPER 1500 // Supefield weite Entfernung
#define SPEED_SUPER2 1000 // Supefield weite Entfernung schnell

//Hall-Sensoren
#define HALL_P 0.01
#define HALL_I 0.0002
#define HALL_D 0.01


// Dribbler
#define BALLDA_SCHWELLE 5
#define BALLDA_SCHWELLE_MAX 5*BALLDA_SCHWELLE

//Brushless-Dribbler
#define DRIBBLER_PRESET 0
//#define DRIBBLER_POWERSTART 60
#define DRIBBLER_POWERSTART_0 300
#define DRIBBLER_POWERSTART_VOLL 2000
#define DRIBBLER_READY 4000

// Ballweg-Intensität
#define ADC_BALLWEG_AKTIV 320
#define ADC_BALLWEG_PASSIV 2000

#define ADC_BALLINAKTIV 400

//Maximaler Balldistanz-Korrigiert wert
#define ADC_MAX 4000

// ADC
//#define _ADC_ASM
//#define _TIEFPASS_IR 63

// Standard Ball-Distanz/Intensität
#define ballIntens ball_Distanz
//#define ballIntens ball_DistanzWinkel
#define BALL_IN_DRIBB (ballda::check() && ballIntens>3000 && BETRAG(ball_Winkel)<20)

// Ultraschall
#define _US_PID
#define usPID_flaeche_Max 100

//////////////////
/* nicht ändern */
//////////////////

// Betrag
#define BETRAG(zahl) (((zahl)>=0)?(zahl):-(zahl))
// Ball-Winkel
#define ball_WinkelR ball_Winkel
//#define ball_WinkelA (ball_Winkel+phi_jetzt)

//Motoren
#define MAX_PWM 1500
#define MIN_PWM 0

// Maximale Soft-PWM-Wiederholungen
#define Soft_Counter_Max 25

// PID
#define dt ((float)0.015625)

// Kompass
#ifdef _CMPS
	#define CMPS kompass.grad()
#else
	#define CMPS 0
#endif

// Pointer
#define Null_EEPROM ((uint16_t*)0)
#define Torrichtung_EEPROM ((uint16_t*)80)

// Schussdauer
#define KICK_DAUER_LOW 15

//Max Chip
#define MAX_READY (PORTD.IN & (0b1<<5))

/////////////
// Schalter //
//////////////

#define MOTORTASTER SCHALTER(1)
#define LINIENTASTER SCHALTER(5)
#define DISPLAYTASTER SCHALTER(3)
#define EEPROMTASTER (!(PORTH.IN & (1 << 6)))
#define LICHTSCHRANKE (PORTK.IN & (1 << 1)) 
#define SW_LINKS SCHALTER(2)
#define SW_RECHTS SCHALTER(4)

#define SCHALTER(in) (PORTH.IN & (1 << (in-1)))

#define DIP(in) (PORTQ.IN & (1 << (in-1)))


////////
// US //  
////////

#define US_RECHTS	0xE2
#define US_HINTEN	0xE4
#define US_LINKS	0xE6

/////////////
//	Kamera //
/////////////
#define PIXIE_I2C 0xa8

#ifdef _PIXIE
	#define NO_OBJECT (kein_tor_counter>200)
#else
	#define NO_OBJECT 1
#endif

/////////////
//	IMU	   //
/////////////
#define IMU_I2C 0x50


/////////////////////
// Spannungsmesser //
/////////////////////
#define SPANNUNG_I2C 0xCE

/////////////////
// PORT-MAKROS //
/////////////////

#define SETLED(led) (PORTJ.OUTSET = (1 << (led-1)))
#define CLEARLED(led) (PORTJ.OUTCLR = (1 << (led-1)))
#endif
