// usart.h: Implementation für USART Schnitstellen

#ifndef _USART_IMP_
#define _USART_IMP_

// Für C++-Files
#ifdef __cplusplus
	extern "C" {
#endif

////////////////
// Kommentare //
////////////////

/*
USART Interrupts:

// DataReg-Empty -> weiteres Byte senden
ISR(USARTC0_DRE_vect)
ISR(USARTC1_DRE_vect)
ISR(USARTD0_DRE_vect)
ISR(USARTD1_DRE_vect)
ISR(USARTE0_DRE_vect)
ISR(USARTE1_DRE_vect)
ISR(USARTF0_DRE_vect)
ISR(USARTF1_DRE_vect)
{
	USART_DataRegEmpty(&USART_data);
}

// Rx-Complete -> Byte empfangen
ISR(USARTC0_RXC_vect)
ISR(USARTC1_RXC_vect)
ISR(USARTD0_RXC_vect)
ISR(USARTD1_RXC_vect)
ISR(USARTE0_RXC_vect)
ISR(USARTE1_RXC_vect)
ISR(USARTF0_RXC_vect)
ISR(USARTF1_RXC_vect)
{
	USART_RXComplete(&USART_data);
}

Alternativ kann beim zweiten Interrupt anstatt des Interrupt-Handlers auch direkt eine Datenverarbeitung programmiert werden
*/

//////////////////
// Definitionen //
//////////////////

// USART-Baudrate berechnen
#define USART_BAUD_SELECT(baudRate, xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define USART_BAUD_SELECT_DOUBLE_SPEED(baudRate, xtalCpu) (((xtalCpu)/((baudRate)*8l)-1) | 0x8000)

// interner USART-Buffer
#define USART_RX_BUFFER_SIZE 16
#define USART_TX_BUFFER_SIZE 16
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

// Buffer-Konsistenz-Check
#if (USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif
#if (USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

// Macro zum internen setzen der Baudrate
#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor) (_usart)->BAUDCTRLA = (uint8_t)_bselValue; (_usart)->BAUDCTRLB = (_bScaleFactor<<USART_BSCALE0_bp) | (_bselValue>>8)

// Macros zum Aktivieren & Deaktivieren der USART-Funktionen
#define USART_Rx_Enable(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)
#define USART_Rx_Disable(_usart) ((_usart)->CTRLB &= ~USART_RXEN_bm)
#define USART_Tx_Enable(_usart) ((_usart)->CTRLB |= USART_TXEN_bm)
#define USART_Tx_Disable(_usart) ((_usart)->CTRLB &= ~USART_TXEN_bm)

// String aus Programmspeicher senden
#define usart_puts_P(__s) usart_puts_p(PSTR(__s))

////////////
// Header //
////////////

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

////////////////
// Konstanten //
////////////////

// USART Datenbuffer
typedef struct USART_Buffer
{
	volatile char RX[USART_RX_BUFFER_SIZE];
	volatile char TX[USART_TX_BUFFER_SIZE];
	volatile uint16_t RX_Head;
	volatile uint16_t RX_Tail;
	volatile uint16_t TX_Head;
	volatile uint16_t TX_Tail;
} USART_Buffer_t;

// Datenstruktur für USART-Interface
typedef struct Usart_and_buffer
{	
	USART_t * usart;
	USART_DREINTLVL_t dreIntLevel;
	USART_Buffer_t buffer;
} USART_data_t;

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Initialisieren
void usart_init(USART_data_t* usart_data, USART_t* usart, unsigned int baudrate, USART_DREINTLVL_t dreIntLevel, USART_RXCINTLVL_t rxcIntLevel, bool selectPort);

// Transaktionen
bool usart_putc(USART_data_t* usart_data, char data); // 71 cycles (including call & return)
unsigned char usart_getc(USART_data_t* usart_data);

// Interrupt-Handler
bool USART_RXComplete(USART_data_t* usart_data);
void USART_DataRegEmpty(USART_data_t* usart_data);

//////////////////////
// Inlinefunktionen //
//////////////////////

// Wurden Daten empfangen
inline bool __attribute__ ((always_inline)) USART_RXBufferData_Available(USART_data_t* usart_data)
{
	// Buffer überprüfen
	uint8_t tempHead = usart_data->buffer.RX_Head;
	uint8_t tempTail = usart_data->buffer.RX_Tail;
	return (tempHead != tempTail);
}

// String senden     
inline void __attribute__ ((always_inline)) usart_puts(USART_data_t* usart_data, const char* str)
{
    while(*str) 
		usart_putc(usart_data, *str++);
}

// String aus Programmcode senden
inline void __attribute__ ((always_inline)) uart_puts_p(USART_data_t* usart_data, const char* progmem_s)
{
	register char data;
    while((data = pgm_read_byte(progmem_s++))) 
		usart_putc(usart_data, data);
}

// Für C++-Files
#ifdef __cplusplus
	}
#endif

#endif
