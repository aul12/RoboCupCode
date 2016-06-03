// global.hpp: globale Variablen

#ifndef _GLOBAL_INC_
#define _GLOBAL_INC_

//////////////////
// Definitionen //
//////////////////

#ifndef _MAIN_FILE_
#error global.hpp shall only be included from main.cpp
#endif

///////////////
// Variablen //
///////////////

// Soft-PWM
volatile uint8_t Soft_Counter = 0;			// Zähler für Soft-PWM

// EEPROM
bool EEPROM_write = true;					// Neuen Wert in EEPROM schreiben

// PID
volatile int16_t k[4];                      // Array für Geschwindigkeit der Motoren (ohne Drehung)
volatile int16_t PID_int;					// endgültiger PID-Wert
volatile int16_t phi_jetzt = 0;             // Winkel für PID-Regelung
volatile int16_t soll_phi = 0;              // Soll-Winkel für PID-Regelung
uint8_t PID_Counter = 0;                    // Zähler für PID-Regelung
volatile int16_t Torrichtung = 0;           // Winkel für Torrichtung
int16_t delta_phi_alt = 0;                  // Differenz der Winkel für PID-Regelung
int16_t delta_phi = 0;                      // Differenz der Winkel für PID-Regelung
float PID_flaeche = 0;                      // Integral der PID-Regelung
volatile int16_t roll = 0;					// Drehrate (D)
float PID = 0;                              // Korrektur der Fahrtgeschwindigkeit zur Drehung
volatile int16_t super_turn = 0;			// Superfielddrehung
volatile uint8_t trick_shoot_turn = 0;		//State Machine für Trickshoot
volatile int16_t _gyroPhi = 0;				//Aus Messwerten, um 1/0.06 größer als GryoPhi
volatile int16_t gyroPhi = 0;

volatile int16_t tor_winkel = 0;			// Absoluter Winkel zum Tor


// ADC
volatile uint8_t muxIR = 0;				    // Aktueller ADC-Kanal
volatile uint16_t ADC_Offset_0[] =			// Offset der IR-Sensoren (Werte danach -> <ADC_BALLWEG)
	{251, 404, 392, 297,
		426, 374, 620, 445};
volatile uint16_t ADC_Offset_1[] =		
	{336, 365, 482, 432,
		 442, 294, 490, 382};
volatile uint16_t ADC_Offset[8];			// Offset der IR-Sensoren (Werte danach -> <ADC_BALLWEG)
volatile uint16_t ADC_Werte[8] = { 0 };		// ADC-Werte für IR-Sensoren
volatile uint16_t ADC_Werte_TP[8] = { 0 };	// ADC-Werte für IR-Sensoren (mit Tiefpass)
volatile uint16_t ball_Distanz = 0;			// Distanz vom Roboter zum Ball
volatile uint16_t ball_DistanzWinkel = 0;	// Distanz vom Roboter zum Ball mit Winkelkorrektur
volatile int16_t ball_Winkel = 0;			// Winkel vom Roboter zum Ball (Relativ zum Roboter) (0-360)
volatile int16_t ball_WinkelA = 0;
volatile uint16_t ball_Distanz_alt = 0;		// Alte Distanz
volatile int16_t ball_Winkel_alt = 0;		// Alter Winkel
volatile uint16_t ball_counter = 0;			// Zähler für Ball-Inaktivität
volatile bool torwart2feldspieler = false;	// Torwart wird zu Feldspieler
const uint16_t ADC_Winkel[] =
	{22+45,338+45,293+45,248+45,203+45,158+45,113+45,68+45};// Winkel der IR-Sensoren
volatile uint8_t lego_IR = 0;				// Lego-Sensor IR
volatile int16_t TSOP = 0;					// TSOP-IR-Sensor
volatile uint8_t super_back = 0;			// Super-Field Patroullie

// TSOP
const uint8_t maxChannel[8] = {
	0b11111110,
	0b10111110,
	0b11101110,
	0b10101110,
	0b11011110,
	0b10011110,
	0b11001110,
	0b10001110
};
volatile uint8_t maxMux = 0;
volatile uint8_t tsopMux = 0;
volatile uint16_t maxVal = 0;
volatile uint16_t tsopWerte[8];

// Ultraschall
volatile uint16_t US_Werte[3] = { 0 };		// Distanzen der Ultraschallsensoren (links, hinten, rechts, vorne)
volatile uint16_t Abstand_Linie[5] = {0,25, 15, 15, 5};
volatile uint8_t US_Abfrage = 0;			// Aktueller Sensor
volatile int16_t US_n_pos[2] = { 0 };		// Position des Roboters
int16_t torwart_x = 61;						// soll X-Position für Torwart
volatile uint8_t maxTSOP = 0;

// Korrigierte Position
uint16_t US_rechts_last[3] = { 80, 0, 0 };	// Letzte Messwerte rechts
uint16_t US_links_last[3] = { 80, 0, 0 };	// Letzte Messwerte links
int16_t US_rechts_diff[2] = { 0 };			// Differenz des rechten Störers
int16_t US_links_diff[2] = { 0 };			// Differenz des linken Störers
volatile int16_t US_tw_pos[2] = { 91, 20 };	// Korrigierte Position
uint8_t US_rechts_block = 0;				// Rechter Sensor blockiert
uint8_t US_links_block = 0;					// Linker Sensor blockiert

// Taster
volatile uint8_t SW_pos = 0x00;				// Position des Schalters

// Display
volatile uint8_t display_page = 0;			// Aktuelle Displayseite
volatile uint16_t keypad_last = 0;			// Letzte gedrückte Taste
volatile uint16_t select_page = 0;			// Seitenwahl über Keypad
volatile uint8_t display_counter = 0;		// Zähler um Ausgabe nicht jeden ISR aufzurufen

// Debug
USART_data_t debug;							// BT2 mit FTDI-Chip
uint8_t debugCount = 0;						// Debugging Zähler für USB

//Bluetooth
USART_data_t bluetooth;						// BT für Comm
uint16_t ballGute = 0;						// Aktuelle Güte der Anfahrt
uint8_t ballGuteEmpfang = 0;				// Aktuelle Güte der Anfahrt des anderen Roboters

//Dribbler
volatile uint8_t dribblerInit = 0;			// Dribbler Initialisierung State-Machine Zustand
volatile uint16_t dribblerTime = 0;			// Timer für State-Machine

//Hall-Sensoren
volatile uint8_t hallCounter[4] = {0,0,0,0};
volatile int16_t rpmHall[4] = {0,0,0,0};
volatile int16_t hallPower[4] = {0,0,0,0};
volatile int16_t hallFlaeche[4] = {0,0,0,0};
volatile int16_t lastDelta[4] = {0,0,0,0};
volatile int8_t hallNoRotationCounter[4] = {0,0,0,0};
	
//Spannungsmesser
uint16_t voltage = 0;
int8_t lowVoltageCount = 0;

// Halb Raus Regelung
volatile uint8_t darfHalbRaus = 0;
	
	
#endif