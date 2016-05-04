// usart.c: Softwareschnittstelle für USART

////////////
// Header //
////////////

#include "usart.h"

////////////////
// Funktionen //
////////////////

// USART-Schnittstelle initialisieren
void usart_init(USART_data_t* usart_data, USART_t* usart, unsigned int baudrate, USART_DREINTLVL_t dreIntLevel, USART_RXCINTLVL_t rxcIntLevel, bool selectPort)
{
	// Ports schalten
	if(selectPort) {
		if(usart == &USARTC0) {
			PORTC.DIRSET = PIN3_bm;
			PORTC.DIRCLR = PIN2_bm;
		}
		else if(usart == &USARTC1) {
			PORTC.DIRSET = PIN7_bm;
			PORTC.DIRCLR = PIN6_bm;
		}
		else if(usart == &USARTD0) {
			PORTD.DIRSET = PIN3_bm;
			PORTD.DIRCLR = PIN2_bm;
		}
		else if(usart == &USARTD1) {
			PORTD.DIRSET = PIN7_bm;
			PORTD.DIRCLR = PIN6_bm;
		}
		else if(usart == &USARTE0) {
			PORTE.DIRSET = PIN3_bm;
			PORTE.DIRCLR = PIN2_bm;
		}
		else if(usart == &USARTE1) {
			PORTE.DIRSET = PIN7_bm;
			PORTE.DIRCLR = PIN6_bm;
		}
		else if(usart == &USARTF0) {
			PORTF.DIRSET = PIN3_bm;
			PORTF.DIRCLR = PIN2_bm;
		}
		else if(usart == &USARTF1) {
			PORTF.DIRSET = PIN7_bm;
			PORTF.DIRCLR = PIN6_bm;
		}
	}
	
	// USART-Interrupts vorbereiten
	usart_data->usart = usart;
	usart_data->dreIntLevel = dreIntLevel;

	// Buffer leeren
	usart_data->buffer.RX_Tail = 0;
	usart_data->buffer.RX_Head = 0;
	usart_data->buffer.TX_Tail = 0;
	usart_data->buffer.TX_Head = 0;
	
	// USART konfigurieren
	usart->CTRLC = (0b00 << 6) | (0b00 << 4) | (0b0 << 3) | (0b011 << 0); // CMODE (async) | Parity (none) | Stop bits (1) | Charsize (8bit)
	usart->CTRLA = rxcIntLevel & (0b11 << 4); // RecieveComplete Interrupt aktivieren
	
	// Baudrate einstellen
	if(baudrate & 0x8000) {
		usart->CTRLB |= USART_CLK2X_bm; // Doublespeed Modus
		baudrate &= ~0x8000;
		USART_Baudrate_Set(usart, baudrate , 0); 
	}		
	else {
		USART_Baudrate_Set(usart, baudrate , 0); 
	}
	
	// Funktionen aktivieren
	USART_Rx_Enable(usart_data->usart);
	USART_Tx_Enable(usart_data->usart);
}

// Byte senden
bool usart_putc(USART_data_t* usart_data, char data)
{
	// Buffer überprüfen
	USART_Buffer_t* TXbufPtr = &usart_data->buffer;
	uint8_t tempHead = (usart_data->buffer.TX_Head + 1) & USART_TX_BUFFER_MASK;
	uint8_t tempTail = usart_data->buffer.TX_Tail;
	bool TXBuffer_FreeSpace = (tempHead != tempTail);

	// Wenn noch Platz im Transmitbuffer
	if(TXBuffer_FreeSpace) {
		// Daten an Bufferspitze schreiben
	  	uint8_t tempTX_Head = TXbufPtr->TX_Head;
	  	TXbufPtr->TX[tempTX_Head] = data;
		TXbufPtr->TX_Head = (tempTX_Head + 1) & USART_TX_BUFFER_MASK;

		// Sendeinterrupt aktivieren
		uint8_t tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | usart_data->dreIntLevel;
		usart_data->usart->CTRLA = tempCTRLA;
	}
	
	// Rückgabe Zustand
	return TXBuffer_FreeSpace;
}

// Byte lesen
unsigned char usart_getc(USART_data_t* usart_data)
{
	// Buffer auslesen
	USART_Buffer_t* bufPtr = &usart_data->buffer;
	uint8_t ans = (bufPtr->RX[bufPtr->RX_Tail]);

	// Ringbuffer verkleinern
	bufPtr->RX_Tail = (bufPtr->RX_Tail + 1) & USART_RX_BUFFER_MASK;
	return ans;
}

// Recieve-Complete-Interrupt-Handler -> Byte empfangen und in Buffer schreiben
bool USART_RXComplete(USART_data_t* usart_data)
{
	// Variablen
	bool ans;
	uint8_t data = usart_data->usart->DATA;
	USART_Buffer_t* bufPtr = &usart_data->buffer;
	
	// Speicherüberprüfung
	uint8_t tempRX_Head = (bufPtr->RX_Head + 1) & USART_RX_BUFFER_MASK;
	uint8_t tempRX_Tail = bufPtr->RX_Tail;
	if(tempRX_Head == tempRX_Tail) {
		// Empfangene Daten verwerfen
	  	ans = false;
	}
	else {
		// Empfangene Daten speichern
		usart_data->buffer.RX[usart_data->buffer.RX_Head] = data;
		usart_data->buffer.RX_Head = tempRX_Head;
		ans = true;
	}
	
	// Ergebnis zurückgeben
	return ans;
}

// Dataregister-Empty-Interrupt-Handler -> weiteres Byte aus Buffer senden
void USART_DataRegEmpty(USART_data_t* usart_data)
{
	// Variablen
	USART_Buffer_t* bufPtr = &usart_data->buffer;
	uint8_t tempTX_Tail = usart_data->buffer.TX_Tail;
	
	// Speicherüberprüfung
	if(bufPtr->TX_Head == tempTX_Tail) {
		// keine Daten mehr -> Interrupt deaktivieren
		uint8_t tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
		usart_data->usart->CTRLA = tempCTRLA;

	}
	else {
		// Daten senden und aus Buffer löschen
		uint8_t data = bufPtr->TX[usart_data->buffer.TX_Tail];
		usart_data->usart->DATA = data;
		bufPtr->TX_Tail = (bufPtr->TX_Tail + 1) & USART_TX_BUFFER_MASK;
	}
}
