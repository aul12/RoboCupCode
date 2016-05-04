// main.c: CoPlatine

///////////////
// Portliste //
///////////////

/*
Stecker 1 -> JTAG 3
Stecker 2 -> JTAG 4
Stecker 3 -> JTAG 1
Stecker 4 -> JTAG 9
Stecker 5 -> JTAG 6
Stecker 6 -> JTAG 0
*/

/*
PORTB0 -> LED 4
PORTB1 -> LED 2
PORTB2 -> SlaveSelect
PORTB3 -> MOSI
PORTB4 -> MISO
PORTB5 -> SCK
PORTC0 -> SSTRB B
PORTC1 -> CS B
PORTC2 -> SSTRB A
PORTC3 -> CS A
PORTC4 -> SDA
PORTC5 -> SCL
PORTD0 -> USART Rx
PORTD1 -> USART Tx
PORTD2 -> LED 1
PORTD3 -> SSTRB D
PORTD4 -> CS D
PORTD5 -> SSTRB C
PORTD6 -> CS C
PORTD7 -> LED 8
*/

/////////////
// Defines //
/////////////

// Debugging
//#define DEBUG_WITHOUT_INTERRUPT
//#define DEBUG_ADC
//#define DEBUG_SCHWELLE
//#define DEBUG_LINIE
//#define DEBUG_TRANSMIT

// Chipselect
#define SPI_CS_A PC3
#define SPI_CS_B PC1
#define SPI_CS_C PD6
#define SPI_CS_D PD4

// Chipready (SSTRB)
#define SPI_ready_A PC2
#define SPI_ready_B PC0
#define SPI_ready_C PD5
#define SPI_ready_D PD3

// SPI Ports
#define SPI_PORT_AB PORTC
#define SPI_PORT_CD PORTD
#define SPI_PIN_AB PINC
#define SPI_PIN_CD PIND

// LEDs
#define LED_1 PD2
#define LED_2 PB1
#define LED_4 PB0
#define LED_8 PD7
#define LED_PORT_18 PORTD
#define LED_PORT_24 PORTB

// EEPROM
#define EEPROM_PTR (128)

// Makros
#define SETPORT(PORT, PIN) (PORT |= (1 << PIN))
#define CLEARPORT(PORT, PIN) (PORT &= ~(1 << PIN))

// LED Makros
#define SET_LED1 (SETPORT(LED_PORT_18, LED_1))
#define SET_LED2 (SETPORT(LED_PORT_24, LED_2))
#define SET_LED4 (SETPORT(LED_PORT_24, LED_4))
#define SET_LED8 (SETPORT(LED_PORT_18, LED_8))
#define CLEAR_LED1 (CLEARPORT(LED_PORT_18, LED_1))
#define CLEAR_LED2 (CLEARPORT(LED_PORT_24, LED_2))
#define CLEAR_LED4 (CLEARPORT(LED_PORT_24, LED_4))
#define CLEAR_LED8 (CLEARPORT(LED_PORT_18, LED_8))

////////////
// Header //
////////////

// Standardheader
#include <avr/io.h>			// I/O - Ports
#include <avr/interrupt.h>	// ISR-Interrupt
#include <avr/eeprom.h>		// EEPROM Speicher
#include <avr/wdt.h>		// Watchdog
#include <util/delay.h>		// Delay Cycles
#include <stdbool.h>		// boolsche Variablen

// Module
#include "uart.h"			// UART Schnittstelle
#include "twimaster.h"		// I≤C Schnittstelle
#include "lcd03.h"			// Display

////////////////
// Konstanten //
////////////////

// SPI-Kommando zum Auslesen
const uint8_t channel[8] = { 
	0b11111110,
	0b10111110,
	0b11101110,
	0b10101110,
	0b11011110,
	0b10011110,
	0b11001110,
	0b10001110
};

///////////////
// Variablen //
///////////////

// Multiplexer
volatile uint8_t mux = 0;				// Befehlsmultiplexer
volatile uint8_t mux_channel = 0;		// Kanalmutiplexer
volatile uint8_t mux_chip = 0;			// Chipmultiplexer

// Daten
volatile uint8_t data[32] = { 0 };		// ADC-Daten
volatile uint8_t trans[5] = { 0 };		// verarbeitete Werte
volatile uint8_t schwelle[32] = { 0 };	// Messschwelle (gr¸n -> weiﬂ)
	
// Kalibration
volatile uint8_t kalibration = 0;
volatile uint8_t schwelle_min[32];
volatile uint8_t schwelle_max[32];

//////////////////////
// Inlinefunktionen //
//////////////////////

// Chipauswahl
inline void __attribute__ ((always_inline)) select_chip(uint8_t chip)
{
	switch(chip) {
		case 0:
			CLEARPORT(SPI_PORT_AB, SPI_CS_A);
			break;
		case 1:
			CLEARPORT(SPI_PORT_AB, SPI_CS_B);
			break;
		case 2:
			CLEARPORT(SPI_PORT_CD, SPI_CS_C);
			break;
		case 3:
			CLEARPORT(SPI_PORT_CD, SPI_CS_D);
			break;
	};
}
inline void __attribute__ ((always_inline)) unselect_chip(uint8_t chip)
{
	switch(chip) {
		case 0:
			SETPORT(SPI_PORT_AB, SPI_CS_A);
			break;
		case 1:
			SETPORT(SPI_PORT_AB, SPI_CS_B);
			break;
		case 2:
			SETPORT(SPI_PORT_CD, SPI_CS_C);
			break;
		case 3:
			SETPORT(SPI_PORT_CD, SPI_CS_D);
			break;
	};
}

// Chipready
inline bool __attribute__ ((always_inline)) chip_ready(uint8_t chip)
{
	switch(chip) {
		case 0:
			return (bool)(SPI_PIN_AB & (1 << SPI_ready_A));
		case 1:
			return (bool)(SPI_PIN_AB & (1 << SPI_ready_B));
		case 2:
			return (bool)(SPI_PIN_CD & (1 << SPI_ready_C));
		case 3:
			return (bool)(SPI_PIN_CD & (1 << SPI_ready_D));
		default:
			return false;
	};
}

///////////////
// Interrupt //
///////////////

// SPI-Transfer fertig
ISR(SPI_STC_vect)
{
	// Daten lesen
	unselect_chip(mux_chip);
	if(mux)
		data[mux_channel+8*mux_chip] = SPDR;
	mux ^= 1;
	
	// Warten & neu starten
	if(mux == 1) { // 2. Befehl
		while(!chip_ready(mux_chip));
		select_chip(mux_chip);
		SPDR = 0;
	}
	else { // n‰chste Messung
		++mux_channel;
		if(mux_channel == 8) {
			mux_channel = 0;
			mux_chip = (mux_chip+1) & 0b11;
		}
		if(mux_chip != 0 || mux_channel != 0) {
			select_chip(mux_chip);
			SPDR = channel[mux_channel];
		}
	}
}

////////////////
// Funktionen //
////////////////

// Hauptprogramm
int main(void)
{
	// Watchdog zur¸cksetzen
	wdt_reset();
	wdt_enable(WDTO_250MS);
	
	// Ports konfigurieren
	DDRB = (1 << LED_2) | (1 << LED_4) | (0b1011 << 2);
	DDRC = (1 << SPI_CS_A) | (1 << SPI_CS_B) | (0b11 << 4);
	DDRD = (1 << SPI_CS_C) | (1 << SPI_CS_D) | (1 << LED_1) | (1 << LED_8);
	
	// Pins konfigurieren
	PORTB |= (1 << PB2);
	
	// Watchdog
	if(MCUSR & (1 << WDRF)) {
		SET_LED8;
		MCUSR &= ~(1 << WDRF);
	}
	
	// Chipselect
	unselect_chip(0);
	unselect_chip(1);
	unselect_chip(2);
	unselect_chip(3);
	
	
	// SPI konfigurieren
	SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (1 << SPR0);
	SPSR = (1 << SPI2X);
	
	// EEPROM lesen
	eeprom_read_block((void*)schwelle, (const uint16_t*)0, 4);
	eeprom_read_block((void*)schwelle, (const void*)EEPROM_PTR, 32);
	
	// I≤C initialisieren
	i2c_init();
	
	// UART einrichten 76,8kHz
	uart_init(UART_BAUD_SELECT(76800, F_CPU));
	
	// Interrupts aktivieren
	sei();
	
	// Daten senden
	while(true) {
		// Watchdog
		wdt_reset();
		
		// Messung starten
		if(mux_chip == 0 && mux_channel == 0) {
			// 1. Messung starten
			select_chip(0);
			SPDR = channel[0];
		}
	
		// Warten -> Messungen
		_delay_ms(3);
		
		// Array anlegen
		trans[0] = (0b10 << 6);
		for(uint8_t i=0; i<6; ++i)
			trans[0] |= (((data[i] > schwelle[i]) ? 1 : 0) << i);
		trans[1] = (0b0 << 7);
		for(uint8_t i=6; i<13; ++i)
			trans[1] |= (((data[i] > schwelle[i]) ? 1 : 0) << (i-6));
		trans[2] = (0b00 << 6);
		for(uint8_t i=13; i<19; ++i)
			trans[2] |= (((data[i] > schwelle[i]) ? 1 : 0) << (i-13));
		trans[3] = (0b0 << 7);
		for(uint8_t i=19; i<26; ++i)
			trans[3] |= (((data[i] > schwelle[i]) ? 1 : 0) << (i-19));
		trans[4] = (0b11 << 6);
		for(uint8_t i=26; i<32; ++i)
			trans[4] |= (((data[i] > schwelle[i]) ? 1 : 0) << (i-26));
			
		// Daten senden
		for(uint8_t i=0; i<5; ++i)
			uart_putc(trans[i]);
			
		// Kalibration
		if(uart_getc(0) == 0b10101010) {
			if(kalibration) {
				// Werte ‰ndern
				for(uint8_t i=0; i<32; ++i) {
					if(data[i] < schwelle_min[i])
						schwelle_min[i] = data[i];
					if(data[i] > schwelle_max[i])
						schwelle_max[i] = data[i];
				}
			}
			else {
				// 1. Aufruf -> Werte vorbereiten
				kalibration = 1;
				for(uint8_t i=0; i<32; ++i) {
					schwelle_min[i] = data[i];
					schwelle_max[i] = data[i];
				}
				SET_LED1;
			}
		}
		else if(uart_getc(0) == 0b01010101) {
			if(kalibration) {
				// Werte ‰ndern
				for(uint8_t i=0; i<32; ++i) {
					if(data[i] > schwelle_max[i])
						schwelle_max[i] = data[i];
				}
			}
			else {
				// 1. Aufruf -> Werte vorbereiten
				kalibration = 2;
				for(uint8_t i=0; i<32; ++i) {
					schwelle_max[i] = data[i];
				}
				SET_LED2;
			}
		}
		else if(kalibration == 1) {
			// Daten speichern
			kalibration = 0;
			for(uint8_t i=0; i<32; ++i) {
				schwelle[i] = (2*schwelle_min[i] + schwelle_max[i]) / 3;
			}
			cli();
			eeprom_write_block((const void*)schwelle, (void*)EEPROM_PTR, 32);
			sei();
			CLEAR_LED1;
		}
		else if(kalibration == 2) {
			// Daten speichern
			kalibration = 0;
			for(uint8_t i=0; i<32; ++i) {
				schwelle[i] = schwelle_max[i] + 5;
			}
			cli();
			eeprom_write_block((const void*)schwelle, (void*)EEPROM_PTR, 32);
			sei();
			CLEAR_LED2;
		}
		
		// Debug
		#ifdef DEBUG_WITHOUT_INTERRUPT
			cli();
			lcd_light(1);
		#endif
		#ifdef DEBUG_ADC
			SET_LED4;
			clear_lcd();
			for(uint8_t x=0; x<8; ++x) {
				uint8_t col = (2 * x) + ((x >> 1) & 1) + (((x >> 2) & 1) * 2) + 1;
				for(uint8_t y=1; y<=4; ++y) {
					display_output_int(y, col, data[4*x + y - 1]);
				}
			}
			_delay_ms(20);
		#endif
		#ifdef DEBUG_SCHWELLE
			SET_LED4;
			clear_lcd();
			for(uint8_t x=0; x<8; ++x) {
				uint8_t col = (2 * x) + ((x >> 1) & 1) + (((x >> 2) & 1) * 2) + 1;
				for(uint8_t y=1; y<=4; ++y) {
					display_output_int(y, col, schwelle[4*x + y - 1]);
				}
			}
			_delay_ms(20);
		#endif
		#ifdef DEBUG_LINIE
			SET_LED4;
			clear_lcd();
			cursor_x_y(1, 11);
			for(uint8_t i=0; i<7; ++i)
				write_char(((data[i] > schwelle[i]) ? 1 : 0) + 0x30);
			cursor_x_y(2, 18);
			write_char(((data[7] > schwelle[7]) ? 1 : 0) + 0x30);
			cursor_x_y(3, 18);
			write_char(((data[8] > schwelle[8]) ? 1 : 0) + 0x30);
			cursor_x_y(4, 4);
			for(uint8_t i=22; i>8; --i)
				write_char(((data[i] > schwelle[i]) ? 1 : 0) + 0x30);
			cursor_x_y(3, 3);
			write_char(((data[23] > schwelle[23]) ? 1 : 0) + 0x30);
			cursor_x_y(2, 3);
			write_char(((data[24] > schwelle[24]) ? 1 : 0) + 0x30);
			cursor_x_y(1, 4);
			for(uint8_t i=25; i<32; ++i)
				write_char(((data[i] > schwelle[i]) ? 1 : 0) + 0x30);
			_delay_ms(20);
		#endif
		#ifdef DEBUG_TRANSMIT
			SET_LED4;
			clear_lcd();
			display_output_num(1, 1, trans[0], 2);
			display_output_num(2, 1, trans[1], 2);
			display_output_num(3, 1, trans[2], 2);
			display_output_num(4, 1, trans[3], 2);
			display_output_num(1, 11, trans[4], 2);
			_delay_ms(20);
		#endif
		#ifdef DEBUG_WITHOUT_INTERRUPT
			sei();
		#endif
	}
}
