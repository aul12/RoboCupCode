// uart.h: UART Schnittstelle (abgeändert)

#ifndef _UART_IMP_
#define _UART_IMP_

//////////////////
// Definitionen //
//////////////////

// Baudrate
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)

// Transmit Buffer
#define UART_TX_BUFFER_SIZE 16
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

// Errorcodes für Recieve
#define UART_FRAME_ERROR 0x08
#define UART_OVERRUN_ERROR 0x04
#define UART_NO_DATA 0x01
#define UART_NO_ERROR 0x00

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Initialisieren
extern void uart_init(uint16_t baudrate);

// Daten verarbeiten
extern uint8_t uart_getc(uint8_t* error);
extern void uart_putc(uint8_t data);

#endif
