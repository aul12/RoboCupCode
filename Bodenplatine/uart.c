// uart.c: UART-Schnittstelle (abgeändert)

////////////
// Header //
////////////

#include <avr/io.h>			// I/O - Ports
#include <avr/interrupt.h>	// ISR - Interrupts
#include <stdint.h>			// standard Variablen
#include "uart.h"			// UART Definitionen

///////////////
// Variablen //
///////////////

static volatile uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf = 0;
static volatile uint8_t UART_TxHead = 0;
static volatile uint8_t UART_TxTail = 0;
static volatile uint8_t UART_LastRxError = UART_NO_DATA;

////////////////
// Interrupts //
////////////////

// UART Recieve Complete Interrupt
ISR(USART_RX_vect)
{
	// Daten und Fehler speichern
    UART_RxBuf = UDR0;
    UART_LastRxError = (UCSR0A & (_BV(FE0) | _BV(DOR0)));
}

// UART Data Register Empty Interrupt
ISR(USART_UDRE_vect)
{
    // Buffer überprüfen
    if(UART_TxHead != UART_TxTail) {
		// Buffer erhöhen
		uint8_t tmptail = UART_TxTail;
        UART_TxTail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
		
		// Daten senden
        UDR0 = UART_TxBuf[tmptail];
    }
	else {
        // Buffer leer -> Interrupt deaktivieren
        UCSR0B &= ~_BV(UDRIE0);
    }
}

////////////////
// Funktionen //
////////////////

// UART initialisieren
void uart_init(uint16_t baudrate)
{
    // Baudrate einstellen
    if(baudrate & 0x8000) {
		// Double Speed
   		UCSR0A = (1 << U2X0);
   		baudrate &= ~0x8000;
   	}
    UBRR0H = (uint8_t)(baudrate >> 8);
    UBRR0L = (uint8_t)baudrate;

    // UART Reciever & Transmitter aktivieren und Recieve Interrupt aktivieren
    UCSR0B = _BV(RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
    
	// Format setzen (8bit, 1 stop, no parity)
    UCSR0C = (0b11 << UCSZ00);
	
	// Sende Interrupt aktivieren
    UCSR0B |= _BV(UDRIE0);
}

// Byte lesen
uint8_t uart_getc(uint8_t* error)
{
	// Fehler ausgeben
	if(error)
		*error = UART_LastRxError;
	
	// Daten zurückgeben
	return UART_RxBuf;
}

// Byte senden
void uart_putc(uint8_t data)
{
	// Buffer überprüfen
	uint8_t tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	while(tmphead == UART_TxTail); // warten solange Buffer voll ist
    
	// Daten in Buffer schreiben
	UART_TxBuf[UART_TxHead] = data;
	UART_TxHead = tmphead;
	
	// Sende-Interrupt aktivieren
	UCSR0B |= _BV(UDRIE0);
}
