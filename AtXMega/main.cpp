// main.cpp: Hauptprojektdatei für Roboterprogramm

//////////////////
// Definitionen //
//////////////////

#define _MAIN_FILE_

////////////
// Header //
////////////

// Standardheader
extern "C" {
	#include <avr/io.h>			// I/O - Ports
	#include <avr/interrupt.h>	// ISR - Interrupt
	#include <avr/eeprom.h>		// Electronically Erasable Presistent Read Only Memory
	#include <avr/wdt.h>		// Watchdog / automatischer Reset
	#include <util/delay.h>		// Delay Cycles
}

// Module/Sensoren
#include "driver/twimaster.h"	// Master I²C Komunikation
#include "driver/usart.h"		// USART Kommunikation
#include "sensor/lcd03.hpp"		// Steuerung für LCD-Display
#include "sensor/srf08.hpp"		// Ultraschallmessung
#include "sensor/pixie.hpp"		// Kamera
#include "sensor/bno055.hpp"	// IMU
#include "sensor/ltc2945.hpp"	// Spannungsmesser
#include "sensor/cmps03.hpp"
#include "libs/atmega8.hpp"		// Atmega8 CoBoard
#include "libs/pixy.hpp"
#include "libs/periphery.hpp"	// Peripheriegeräte

// Eigene Header
#include "defines.h"			// globale Definitionen
#include "global.hpp"			// globale Variablen
#include "libs/sinus.h"			// Eigene Sinusfunktion
#include "libs/motor.hpp"		// Motorfunktionen
#include "libs/ir.hpp"			// Infrarotsensorfunktionen
#include "libs/debug.h"			//Debugging via USB

/////////////
// Klassen //
/////////////

TWI_Master_t twiMaster;
TWI_Master_t twiMasterD;
TWI_Master_t twiMasterF;
lcd03 display(&twiMasterF, I2C_DISPLAY);
srf08 US_sensor[3] = { srf08(&twiMasterF, US_RECHTS), srf08(&twiMasterF, US_HINTEN), srf08(&twiMasterF, US_LINKS)};
bno055 imu(&twiMasterD, IMU_I2C);
ltc2945 spannung(&twiMasterF, SPANNUNG_I2C);
Pixie kamera(&twiMasterF, PIXIE_I2C);
cmps03 kompass(&twiMasterD, I2C_CMPS);

////////////////
// Interrupts //
////////////////

#include "interrupts.hpp"

/////////////////////
// Spielfunktionen //
/////////////////////

#include "spiel.hpp"

////////////////
// Funktionen //
////////////////

// Hauptfunktion
int main(void)
{
	// Watchdog
	wdt_reset();
	wdt_enable(WDTO_500MS);

	// System Clock einstellen
	OSC.XOSCCTRL = (0b11 << 6) | (0b0011 << 0); // frequency range (12-16MHz) | Startzeit (256clks)
	OSC.CTRL |= (0b1 << 3); // Clocksource aktivieren (extern)
	OSC.PLLCTRL = (0b11 << 6) | (0b0010 << 0); // PLL input (extern) | PLL Faktor (2)
	while(!(OSC.STATUS & (0b1 << 3))); // Warten bis Clocksource stabil ist (extern)
	OSC.CTRL |= (0b1 << 4); // Clocksource aktivieren (PLL)
	CCP = 0xD8; // CCP Write Protection deaktivieren
	CLK.PSCTRL = (0b00000 << 2) | (0b00 << 0); // Prescal A (1) | Prescaler B & C (1 & 1)
	while(!(OSC.STATUS & (0b1 << 4))); // Warten bis Clocksource stabil ist (PLL)
	CCP = 0xD8; // CCP Write Protection deaktivieren
	CLK.CTRL = (0b100 << 0); // Clocksource wechseln
	
	// I2C initialisieren
	TWI_MasterInit(&twiMasterD, &TWID, TWI_MASTER_INTLVL_HI_gc, TWI_BAUD(F_CPU, 100000UL));
	TWI_MasterInit(&twiMasterF, &TWIF, TWI_MASTER_INTLVL_HI_gc, TWI_BAUD(F_CPU, 100000UL));

	// Ports deklarieren
	PORTB.DIR = 0b00000000;		//Hall und JTAG
	PORTC.DIR = 0b11011011;		//Dribbler, Pixy und SPI
	PORTD.DIR = 0b10001011;		//Pixy, BT, I²C
	PORTE.DIR = 0b10001111;		//Linie, Hall, PWM
	PORTF.DIR = 0b10011011;		//BT, SDCS, Raspi, I²C
	PORTH.DIR = 0b00000000;		//Schalter
	PORTJ.DIR = 0b11111111;		//LEDS
	PORTK.DIR = 0b11111100;		//Dir, Bno-Reset, Schuss, Lichtschranke
	PORTQ.DIR = 0b00000000;		//DIP-Schalter
	
	//Pull-Up für DIP
	PORTQ.PIN0CTRL = (0b011 << 3);
	PORTQ.PIN1CTRL = (0b011 << 3);
	PORTQ.PIN2CTRL = (0b011 << 3);
	PORTQ.PIN3CTRL = (0b011 << 3);
	
	//Pull-Up für Taster (EEPROM)
	PORTH.PIN6CTRL = (0b011 << 3);
	
	//Reset für BNO auf High (K3)
	PORTK.OUTSET = (0b1 << 3);
	
	// PWM-Einstellungen (S. 163)
	TCE0.CTRLA = (0b0001 << 0); // Prescaler (1)
	TCE0.CTRLB = (0b1111 << 4) | (0b011 << 0); // Ports aktivieren (ABCD) | PWM (single-slope)
	TCE0.PER = MAX_PWM; // maximaler PWM-Wert
	
	
	// PWM-Einstellungen für Brushless Treiber (C0) 
	TCC0.CTRLA = 0b0101;//Prescaler(1/64)
	TCC0.CTRLB = (0b0001 << 4) | (0b011<<0); // Port A aktivieren (->C0) | PWM (single-slope)
	TCC0.PER = 10000;//1 PWM Cycle entspricht 20ms (50Hz)
	TCC0.CCA = 500;
	
	PORTC.OUTSET = (0b1<<1);
	
	
	// Interrupts für Hall
	/*PORTB.INTCTRL = (0b11 <<2 )|(0b11 << 0);	//Interruptlevel setzten (High)
	PORTB.PIN0CTRL = (0b001 << 0);			//Interrupt auf Rising-Edge
	PORTB.INT0MASK = (0b1<<0);				//Pin für Interrupt setzten (B0)
	PORTB.INT1MASK = (0b1<<1);				//Pin für Interrupt setzten (B1)
	
	PORTE.INTCTRL = (0b11 <<2 )|(0b11 << 0);	//Interruptlevel setzten (High)
	PORTE.PIN0CTRL = (0b001 << 0);			//Interrupt auf Rising-Edge
	PORTE.INT0MASK = (0b1<<4);				//Pin für Interrupt setzten (E4)
	PORTE.INT1MASK = (0b1<<5);				//Pin für Interrupt setzten (E5)*/

	// Reset-Type
	if(RST.STATUS & (1 << 3)) { // Watchdog
		SETLED(1);
		RST.STATUS = (1 << 3);
	}
	else if(RST.STATUS & (1 << 2)) { // Brownout
		SETLED(2);
		RST.STATUS = (1 << 2);
	}
	else if(RST.STATUS & (1 << 5)) { // Software
		SETLED(1);
		SETLED(2);
		RST.STATUS = (1 << 5);
	}
	else if(RST.STATUS & (1 << 4)) { // Debug
		//SETLED(4);
		//SETLED(6);
		RST.STATUS = (1 << 4);
	}
	else if(RST.STATUS & (1 << 1)) { // Extern
		//SETLED(4);
		RST.STATUS = (1 << 1);
	}
	else if(RST.STATUS & (1 << 0)) { // Power
		//SETLED(6);
		RST.STATUS = (1 << 0);
	}
	
	
	// Overflow Interrupt (Specsheet S.163)
	TCC1.CTRLA = (0b0111 << 0); // Prescaler (1/1024)
	TCC1.CTRLB = (0b000 << 0); // Form (normal)
	TCC1.PER = 100; // Zählerschritte -> 3,2ms
	TCC1.CNT = 0;
	TCC1.INTCTRLA = (0b01 << 0); // Interruptlevel (low)

	//16bit Timer für Hall-Sensoren
	TCD0.CTRLA = (0b0111 << 0);	//Prescaler (1/1024)
	TCD0.CTRLB = (0b000 << 0);	//Form (normal) -> kein PWM
	TCD0.PER = 65535;			//Maximale Zählerschritte (16bit)
	TCD0.CNT = 0;				//Starte Count von 0
	TCD0.INTCTRLA = (0b01 << 0);	//Overflow-Interruptlevel (low)
	
	TCD1.CTRLA = (0b0111 << 0);	//Prescaler (1/1024)
	TCD1.CTRLB = (0b000 << 0);	//Form (normal) -> kein PWM
	TCD1.PER = 65535;			//Maximale Zählerschritte (16bit)
	TCD1.CNT = 0;				//Starte Count von 0
	TCD1.INTCTRLA = (0b01 << 0);	//Overflow-Interruptlevel (low)
	
	TCF0.CTRLA = (0b0111 << 0);	//Prescaler (1/1024)
	TCF0.CTRLB = (0b000 << 0);	//Form (normal) -> kein PWM
	TCF0.PER = 65535;			//Maximale Zählerschritte (16bit)
	TCF0.CNT = 0;				//Starte Count von 0
	TCF0.INTCTRLA = (0b01 << 0);	//Overflow-Interruptlevel (low)
	
	TCF1.CTRLA = (0b0111 << 0);	//Prescaler (1/1024)
	TCF1.CTRLB = (0b000 << 0);	//Form (normal) -> kein PWM
	TCF1.PER = 65535;			//Maximale Zählerschritte (16bit)
	TCF1.CNT = 0;				//Starte Count von 0
	TCF1.INTCTRLA = (0b01 << 0);	//Overflow-Interruptlevel (low)
	

    // Torrichtung bestimmen
	Torrichtung = (int16_t)eeprom_read_word(Null_EEPROM); // Zuweisung irrelevant -> Warnung unterdrücken
	Torrichtung = (int16_t)eeprom_read_word(Torrichtung_EEPROM);
	if(Torrichtung > 3600)
		Torrichtung = 0;
   
    // CoBoard initialisieren
    atmega8_init();
	
	//Debug-USB/BT initialisieren
	usart_init(&debug, &USARTF1, USART_BAUD_SELECT(38400, F_CPU), USART_DREINTLVL_LO_gc, USART_RXCINTLVL_OFF_gc, true);
	
	//Bluetooth initialisieren
	usart_init(&bluetooth, &USARTD0, USART_BAUD_SELECT(38400, F_CPU), USART_DREINTLVL_LO_gc, USART_RXCINTLVL_LO_gc, true);
	
	pixyInit();
	
	
	// Interrupts aktivieren
	cli();
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();
	if(DISPLAYTASTER)
		display.light(true);

	// US_Kanäle initialisieren
	#ifdef _US
		US_sensor[0].init();
		US_sensor[1].init();
		US_sensor[2].init();
		US_sensor[3].init();
		US_sensor[0].start();
		US_sensor[2].start();
	#else
		//Einkommentieren um US umzubennen (nur wenn US nicht benutzt)
		/*change_srf08_addr(&twiMasterF, 0xE6, 0xE4);
		while(1)
			wdt_reset();*/
	#endif

	
	
	
		
		
	// ADC-Interrupt initialisieren
	if(ROBO==0){
		ADC_Offset[0] = ADC_Offset_0[0];
		ADC_Offset[1] = ADC_Offset_0[1];
		ADC_Offset[2] = ADC_Offset_0[2];
		ADC_Offset[3] = ADC_Offset_0[3];
		ADC_Offset[4] = ADC_Offset_0[4];
		ADC_Offset[5] = ADC_Offset_0[5];
		ADC_Offset[6] = ADC_Offset_0[6];
		ADC_Offset[7] = ADC_Offset_0[7];
	}
	else{
		ADC_Offset[0] = ADC_Offset_1[0];
		ADC_Offset[1] = ADC_Offset_1[1];
		ADC_Offset[2] = ADC_Offset_1[2];
		ADC_Offset[3] = ADC_Offset_1[3];
		ADC_Offset[4] = ADC_Offset_1[4];
		ADC_Offset[5] = ADC_Offset_1[5];
		ADC_Offset[6] = ADC_Offset_1[6];
		ADC_Offset[7] = ADC_Offset_1[7];
	}
	ADC_init();

	// Beginn des Hauptalgorithmus
    soll_phi = 0;
	Fahrtrichtung(0, 0);
	
	out = 0;
	
	// Endlosschleife
    while(true) {
		// Watchdog
		wdt_reset();
	
		// Programmzustände
		if(SW_RECHTS) // Rechter Taster unten
			SW_pos = 1;
		else
			SW_pos = 0;
		if(SW_LINKS) // Linker Taster oben
			SW_pos += 2;
		if(torwart2feldspieler && SW_pos == 1) {
			SW_pos = 0;
		}
		
		if(!MOTORTASTER)
			startTimer=0;
		
		
		// Linienreaktion
		#if _COMPLEX_LINE==1||_COMPLEX_LINE==5
			if(out > 2) {
				#ifdef _US_VERIF
					if(US_Werte[0]>30 && US_Werte[1]>30 && US_Werte[2]>30)
						Fahrtrichtung(out_winkel+180, SPEED_LINIE);
				#else
					Fahrtrichtung(out_winkel+180, SPEED_LINIE);
				#endif
			}
		#elif _COMPLEX_LINE==2 
			if(out > 1) {
				#ifdef _US_VERIF
					if(US_Werte[0]>30 && US_Werte[1]>30 && US_Werte[2]>30)
						Fahrtrichtung(out_winkel+180, SPEED_LINIE);
				#else
					Fahrtrichtung(out_winkel+180, SPEED_LINIE);
				#endif
			}
		#elif _COMPLEX_LINE==3||_COMPLEX_LINE==6
			if(out >= 2) {
				#ifdef _US_VERIF
					if(US_Werte[0]>30 && US_Werte[1]>30 && US_Werte[2]>30)
						Fahrtrichtung(-out_winkel, SPEED_LINIE);
				#else
					Fahrtrichtung(-out_winkel, SPEED_LINIE);
				#endif
			}
		#elif _COMPLEX_LINE==4
			if(out >= 1) {
				#ifdef _US_VERIF
					if(US_Werte[0]>30 && US_Werte[1]>30 && US_Werte[2]>30)
						Fahrtrichtung(-out_winkel, SPEED_LINIE);
				#else
					Fahrtrichtung(-out_winkel, SPEED_LINIE);
				#endif
			}
		#elif _COMPLEX_LINE==7
			if(US_pos[1]<30)//hinten
			{
				if(US_pos[0]<40)//links
					Fahrtrichtung(315, SPEED_LINIE);
				else if(US_pos[0]>120)//rechts
					Fahrtrichtung(45, SPEED_LINIE);
				else
					Fahrtrichtung(0, SPEED_LINIE);
			}
			else if(US_pos[1]>140)//vorne
			{
				if(US_pos[0]<40)//links
					Fahrtrichtung(225, SPEED_LINIE);
				else if(US_pos[0]>120)//rechts
					Fahrtrichtung(135, SPEED_LINIE);
				else
					Fahrtrichtung(180, SPEED_LINIE);
			}
			else if(US_pos[0]<40)//links
				Fahrtrichtung(270, SPEED_LINIE);
			else if(US_pos[0]>120)//rechts
				Fahrtrichtung(90, SPEED_LINIE);
		#elif _COMPLEX_LINE==8
			if(out>0)
				Fahrtrichtung(out_winkel, SPEED_LINIE);
		#else
			if(out > 1) {
				Fahrtrichtung(out_winkel+180, SPEED_LINIE);
				wdt_delay(DELAY_LINIE);
				out = 0;
			}
		#endif
			else {
				#ifdef SUPERFIELD
					// Warnung
					#if _COMPLEX_LINE!=3&&_COMPLEX_LINE!=4
						#warning Auf der Linie fahren geht nicht ohne _COMPLEX_LINE=3|4
					#endif
						
					// Superfieldprogramme
					if(SW_pos == 0) { // Superfieldprogramm normal schnell
						spielSuper3();
					} 
					else if(SW_pos == 1) { // Superfieldprogramm normal
						spielSuper2();
					}
					else if(SW_pos == 2) { // Fahrprogramm sofort Schuss
						spielB2();
					}
					else { // Nullprogramm
						PIDprogramm();
						#ifdef _SCHUSS
							if(!MOTORTASTER && ballda::check()) {
								schuss::Kick_LP();
							}
						#endif
						_delay_us(10);
					}
				#else
					// Spielprogramme
					if(SW_pos == 0) { // Normales Fahrprogramm
						spielB1();
					} 
					else if(SW_pos == 1) { // Torwart
						torwartB();
					}
					else if(SW_pos == 2) { // Fahrprogramm sofort Schuss
						spielB2();
					}
					else { // Nullprogramm
						PIDprogramm();
							
						#ifdef _SCHUSS
							if(!MOTORTASTER && ballda::check()) {
								schuss::Kick_LP();
							}
						#endif
						_delay_us(10);
					}
				#endif
			}
		
		//Debugging über USB
		if(DISPLAYTASTER && !MOTORTASTER){
			switch(debugCount ++){
				case 0:
					//ADC
					for(uint8_t c=0; c<8; c++){
						sendData(0xA0 + c, (uint8_t)(ADC_Werte[c]/16));
					}
				break;
				case 1:
					//I²C
					sendData(0xB1, (uint8_t)(roll/256));
					sendData(0xB3, (uint8_t)(US_Werte[0]/2));
					sendData(0xB4, (uint8_t)(US_Werte[1]/2));
					sendData(0xB5, (uint8_t)(US_Werte[2]/2));
				break;
				case 2:
					//Berechnete Werte
					sendData(0xC0, (uint8_t)(ball_Winkel/2));
					sendData(0xC2, (uint8_t)US_pos[0]);
					sendData(0xC3, (uint8_t)US_pos[1]);
					sendData(0xC4, (uint8_t)(soll_phi/2));
					sendData(0xC5, (uint8_t)(phi_jetzt/2));
					sendData(0xC6, SW_pos);
					sendData(0xC7, tor_winkel);
					sendData(0xC8, (uint8_t)(ballIntens/24));
					sendData(0xC9, ballda::check()?1:0);
				break;
				case 3:
					//IO
					sendData(0xD0, MOTORTASTER?1:0);
					sendData(0xD1, DISPLAYTASTER?1:0);
					sendData(0xD2, LINIENTASTER?1:0);
					sendData(0xD3, SW_LINKS?1:0);
					sendData(0xD4, SW_RECHTS?1:0);
					sendData(0xD5, EEPROMTASTER?1:0);
					sendData(0xD6, LICHTSCHRANKE?1:0);
					sendData(0xD7, SCHALTER(6)?1:0);
				break;
				case 4:
					//Motoren
					sendData(0xE0, (uint8_t)((k[0]+PID_int)/8));
					sendData(0xE1, (uint8_t)((k[1]+PID_int)/8));
					sendData(0xE2, (uint8_t)((k[2]+PID_int)/8));
					sendData(0xE3, (uint8_t)((k[3]+PID_int)/8));
					sendData(0xE6, out);
					sendData(0xE7, (uint8_t)(out_winkel/2));
				break;
				default:
					debugCount = 0;
				break;
			}
		}
    }

}
