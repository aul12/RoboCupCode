// ir_asm.S: ADC-Interrupt in Assembler

////////////
// Header //
////////////

#include <avr/io.h>
#include "../defines.h"

//////////////////
// Definitionen //
//////////////////

// Assemblyinterrupt
#ifdef _ADC_ASM
.SET __ADC_ASM, 1
#endif

// Tiefpass nicht möglich
#ifdef _TIEFPASS_IR
#ifdef _ADC_ASM
.SET __TIEFPASS_IR, 1
#endif
#endif
.ifdef __TIEFPASS_IR
.error "IR-Tiefpass wird im ASM-Modus nicht unterstützt!"
.endif

//////////////////////////
// Variablen -> Pointer //
//////////////////////////

.extern muxIR
.extern ADC_Werte
.extern ADC_Offset

///////////////
// Interrupt //
///////////////

.ifdef __ADC_ASM
.GLOBAL	ADCA_CH0_vect
ADCA_CH0_vect: // (87 cycles)
	// Register sichern //
	PUSH r0
	PUSH r1
	CLR r1
	IN r0, 0x3F // SREG
	PUSH r0
	OUT 0x3F, r1
	IN r0, 0x38 // RAMPD
	PUSH r0
	OUT 0x38, r1
	IN r0, 0x3A // RAMPY
	PUSH r0
	OUT 0x3A, r1
	PUSH r23
	PUSH r24
	PUSH r25
	PUSH r28 // YL
	PUSH r29 // YH
	PUSH r30
	PUSH r31

	// letzten Messwert in Array laden //
	LDS r23, muxIR // Multiplexer laden
	LSL r23 // Shift left 1 -> MUL 2
	LDS r24, 0x210 // ADCA CH0 RESL in Register lesen
	LDS r25, 0x211 // ADCA CH0 RESH in Register lesen
	LDI YL, lo8(ADC_Offset) // Pointer laden
	LDI YH, hi8(ADC_Offset) // Pointer laden
	ADD r28, r23 // Adresse um Multiplexer erhöhen (low)
	ADC r29, r1 // Adresse um Multiplexer erhöhen (high)
	LD r30, Y+ // Offset laden (low)
	LD r31, Y // Offset laden (high)
	SUB r24, r30 // Offset abziehen (low)
	SBC r25, r31 // Offset abziehen (high)
	LDI YL, lo8(ADC_Werte) // Pointer laden
	LDI YH, hi8(ADC_Werte) // Pointer laden
	ADD r28, r23 // Adresse um Multiplexer erhöhen (low)
	ADC r29, r1 // Adresse um Multiplexer erhöhen (high)
	ST Y+, r24 // ADC Wert speichern (low)
	ST Y, r25 // ADC Wert speichern (high)

	// Multiplexer erhöhen //
	LSR r23 // Shift right 1 -> DIV 2
	INC r23 // muxIR++
	ANDI r23, 0x7 // muxIR & 0b111
	STS muxIR, r23 // Multiplexer speichern

	// ADC-Kanal wechseln //
	LSL r23 // Shift left 1
	LSL r23 // Shift left 1
	LSL r23 // Shift left 1
	STS 0x221, r23 // ADCA CH0 MUXCTRL speichern

	// Neue Messung starten //
	LDS r23, 0x220 // ADCA CH0 CTRL in Register lesen
	ORI r23, 0x80 // bit 7 setzen
	STS 0x220, r23 // ADCA CH0 CTRL speichern

	// Register wiederherstellen //
	POP r31
	POP r30
	POP r29
	POP r28
	POP r25
	POP r24
	POP r23
	POP r0
	OUT 0x3A, r0
	POP r0
	OUT 0x38, r0
	POP r0
	OUT 0x3F, r0
	POP r1
	POP r0

	RETI
.endif

/////////////////////
// Original C-Code //
/////////////////////

/*

// ADC-Interrupt (118 cycles)
ISR(ADCA_CH0_vect)
{
	// letzten Messwert in Array eintragen
	ADC_Werte[muxIR] = ADCA.CH0.RES-ADC_Offset[muxIR];
	
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

*/
