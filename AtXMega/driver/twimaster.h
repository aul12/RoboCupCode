// twimaster.h: Implementation für TWI I²C-Interface

#ifndef _TWI_MASTER_IMP_
#define _TWI_MASTER_IMP_

// Für C++-Files
#ifdef __cplusplus
	extern "C" {
#endif

////////////////
// Kommentare //
////////////////

/*
Für asynchronen TWI wird pro Schnittstelle ein Interrupt benötigt:

ISR(TWIC_TWIM_vect)
ISR(TWID_TWIM_vect)
ISR(TWIE_TWIM_vect)
ISR(TWIF_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}
*/

//////////////////
// Definitionen //
//////////////////

// Macro zur Erstellung der Baudrate
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)

// Status des TWI-Busses
#define TWIM_STATUS_READY 0
#define TWIM_STATUS_BUSY 1

// Größe des Buffers für asynchronen TWI
#define TWIM_WRITE_BUFFER_SIZE 32
#define TWIM_READ_BUFFER_SIZE 64

////////////
// Header //
////////////

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

////////////////
// Konstanten //
////////////////

// Ergebniss für asynchronen TWI
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = 0,
	TWIM_RESULT_OK               = 1,
	TWIM_RESULT_BUFFER_OVERFLOW  = 2,
	TWIM_RESULT_ARBITRATION_LOST = 3,
	TWIM_RESULT_BUS_ERROR        = 4,
	TWIM_RESULT_NACK_RECEIVED    = 5,
	TWIM_RESULT_FAIL             = 6,
} TWIM_RESULT_t;

// Datenstruktur für TWI-Interface
typedef struct TWI_Master {
	TWI_t *interface;
	register8_t address;
	register8_t writeData[TWIM_WRITE_BUFFER_SIZE];
	register8_t readData[TWIM_READ_BUFFER_SIZE];
	register8_t bytesToWrite;
	register8_t bytesToRead;
	register8_t bytesWritten;
	register8_t bytesRead;
	register8_t status;
	register8_t result;
} TWI_Master_t;

////////////////////////
// Funktionsanmeldung //
////////////////////////

// Initialisieren
void TWI_MasterInit(TWI_Master_t* twi, TWI_t* module, TWI_MASTER_INTLVL_t intLevel, uint8_t baudRateRegisterSetting);

// Bus Zustand abfragen
TWI_MASTER_BUSSTATE_t TWI_MasterState(TWI_Master_t* twi);

// Bus Transaktion durchführen
bool TWI_MasterWriteRead(TWI_Master_t* twi, uint8_t address, uint8_t* writeData, uint8_t bytesToWrite, uint8_t bytesToRead);
bool TWI_MasterWriteNull(TWI_Master_t* twi, uint8_t address);

// Interrupt-Handler
void TWI_MasterInterruptHandler(TWI_Master_t* twi);
void TWI_MasterArbitrationLostBusErrorHandler(TWI_Master_t* twi);
void TWI_MasterWriteHandler(TWI_Master_t* twi);
void TWI_MasterReadHandler(TWI_Master_t* twi);
void TWI_MasterTransactionFinished(TWI_Master_t* twi, uint8_t result);

//////////////////////
// Inlinefunktionen //
//////////////////////

// TWI-Master bereit
inline bool __attribute__ ((always_inline)) TWI_MasterReady(TWI_Master_t* twi)
{
	bool twi_status = (twi->status == TWIM_STATUS_READY);
	return twi_status;
}

// TWI-Master Write-Wrapper
inline bool __attribute__ ((always_inline)) TWI_MasterWrite(TWI_Master_t* twi, uint8_t address, uint8_t *writeData, uint8_t bytesToWrite)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, writeData, bytesToWrite, 0);
	return twi_status;
}

// TWI-Master Read-Wrapper
inline bool __attribute__ ((always_inline)) TWI_MasterRead(TWI_Master_t* twi, uint8_t address, uint8_t bytesToRead)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, 0, 0, bytesToRead);
	return twi_status;
}

// Für C++-Files
#ifdef __cplusplus
	}
#endif

#endif
