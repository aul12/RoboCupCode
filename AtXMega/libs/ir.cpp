// ir.cpp: Funktionen zum Auslesen der Infrarotsensoren

////////////
// Header //
////////////

// Standardheader
extern "C" {
	#include <avr/interrupt.h>	// ISR - Interrupt
	#include <avr/pgmspace.h>	// PGM Funktionen
	#include <stddef.h>			// Standarddefinitionen
}

// Eigene Header
#include "../defines.h"
#include "ir.hpp"

///////////////
// Interrupt //
///////////////

// ADC-Interrupt (118 cycles)
#ifndef _ADC_ASM
extern volatile uint16_t ADC_Werte[8];
extern volatile uint16_t ADC_Werte_TP[8];
extern volatile uint8_t muxIR;
extern volatile uint16_t ADC_Offset[8];
extern volatile uint8_t muxIRB;
extern volatile uint16_t TSOP_Werte[4];
ISR(ADCA_CH0_vect)
{
	// letzten Messwert in Array eintragen
	ADC_Werte[muxIR] = ADCA.CH0.RES-ADC_Offset[muxIR];
	/*if(ROBO == 0 && muxIR == 7)
		ADC_Werte[7] = (ADC_Werte[0] + ADC_Werte[6])/2;*/
	
	// Tiefpass
	#ifdef _TIEFPASS_IR
		ADC_Werte_TP[muxIR] = (ADC_Werte_TP[muxIR]*_TIEFPASS_IR+ADC_Werte[muxIR])/(_TIEFPASS_IR+1);
	#endif
	
	// Multiplexer
	muxIR = (muxIR+1) & 0b111;
	
	// ADC-Kanal wechseln
	ADCA.CH0.MUXCTRL = (muxIR << 3);
	
	// neue Messung starten
	ADCA.CH0.CTRL |= 0x80;
}
#endif

// ADC-Interrupt
extern volatile int16_t TSOP;
ISR(ADCB_CH0_vect)
{
	
	/*// letzten Messwert in Array eintragen
	if(muxIRB==6)
		TSOP_Werte[0] = ADCB.CH0.RES;
	
	else
		TSOP_Werte[muxIRB] = ADCB.CH0.RES;
	
	// Multiplexer
	//muxIRB = (muxIRB+1) & 0b11;
	switch(muxIRB)
	{
		case 1:
			muxIRB =2;
			break;
		case 2:
			muxIRB = 3;
			break;
		case 3:
			muxIRB = 6;
			break;
		case 6:
			muxIRB = 1;
			break;
	}
	
	// ADC-Kanal wechseln
	ADCB.CH0.MUXCTRL = (muxIRB << 3);
	
	// neue Messung starten
	ADCB.CH0.CTRL |= 0x80;*/
}

////////////////
// Funktionen //
////////////////

// Kalibration lesen
int ADC_call(int Index)
{
	int result;
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(Index);
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	return result;
}

// ADC Interrupt initialisieren
void ADC_init(void)
{
	// ADC Initialisieren
	ADCA.CALL = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
	ADCA.CALH = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
	ADCA.CALL = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
	ADCA.CALH = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
	ADCA.CH0.CTRL = (0b0 << 7) | (0b000 << 2) | (0b01 << 0); // Messung starten | Gain (1x) | Modus
	ADCA.CH0.MUXCTRL = (0b0000 << 3); // ADC-Kanal (0)
	ADCA.CTRLB = (0b0 << 4) | (0b0 << 3) | (0b00 << 1); // Modus (unsigned) | Freerun (aus) | Auflösung (12bit)
	ADCA.PRESCALER = (0b110 << 0); // Frequenz (1/256 -> 125kHz)
	ADCA.REFCTRL = (0b11 << 4); // Referenzspannung (REFB)
	ADCA.EVCTRL = 0b00000000; // Kein Eventsystem
	ADCA.INTFLAGS = 0b00001111; // Interrupt-Flags leeren
	ADCA.CH0.INTCTRL = ADC_CH_INTLVL0_bm; // Interrupt-Priorität (Low)
	ADCA.CTRLA = 0b1; // ADC aktivieren
	
	// Messung starten
	ADCA.CH0.CTRL |= 0x80;
	
	// Tatsächliche ADC-Samplerate geringer
	// Specsheet 25.9 -> S. 290
	// Durch einzelnes Auslesen 7 Zyklen
	// Samplerate = 125k / 7 = 17,86k
}

void ADC_initB(void)
{
	/*// ADC Initialisieren
	ADCB.CALL = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));
	ADCB.CALH = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1));
	ADCB.CALL = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));
	ADCB.CALH = ADC_call(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1));
	ADCB.CH0.CTRL = (0b0 << 7) | (0b000 << 2) | (0b01 << 0); // Messung starten | Gain (1x) | Modus
	ADCB.CH0.MUXCTRL = (0b0011 << 3); // ADC-Kanal (3)
	ADCB.CTRLB = (0b0 << 4) | (0b0 << 3) | (0b00 << 1); // Modus (unsigned) | Freerun (aus) | Auflösung (12bit)
	ADCB.PRESCALER = (0b110 << 0); // Frequenz (1/256 -> 125kHz)
	ADCB.REFCTRL = (0b11 << 4); // Refere kzspannung (REFB)
	ADCB.EVCTRL = 0b00000000; // Kein Eventsystem
	ADCB.INTFLAGS = 0b00001111; // Interrupt-Flags leeren
	ADCB.CH0.INTCTRL = ADC_CH_INTLVL0_bm; // Interrupt-Priorität (Low)
	ADCB.CTRLA = 0b1; // ADC aktivieren
	
	// Messung starten
	ADCB.CH0.CTRL |= 0x80;*/
	
	// Tatsächliche ADC-Samplerate geringer
	// Specsheet 25.9 -> S. 290
	// Durch einzelnes Auslesen 7 Zyklen
	// Samplerate = 125k / 7 = 17,86k
}
