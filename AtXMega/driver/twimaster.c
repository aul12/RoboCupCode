// twimaster.c: Softwareschnittstelle für TWI

////////////
// Header //
////////////

#include "twimaster.h"

////////////////
// Funktionen //
////////////////

// TWI-Master initialisieren
void TWI_MasterInit(TWI_Master_t* twi, TWI_t* module, TWI_MASTER_INTLVL_t intLevel, uint8_t baudRateRegisterSetting)
{
	// TWI konfigurieren
	twi->interface = module;
	twi->interface->MASTER.CTRLA = intLevel | TWI_MASTER_RIEN_bm | TWI_MASTER_WIEN_bm | TWI_MASTER_ENABLE_bm;
	twi->interface->MASTER.BAUD = baudRateRegisterSetting;
	twi->interface->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

// TWI-Master Buszustand
TWI_MASTER_BUSSTATE_t TWI_MasterState(TWI_Master_t* twi)
{
	TWI_MASTER_BUSSTATE_t twi_status;
	twi_status = (TWI_MASTER_BUSSTATE_t)(twi->interface->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm);
	return twi_status;
}

// TWI-Master Transaktion durchführen
bool TWI_MasterWriteRead(TWI_Master_t* twi, uint8_t address, uint8_t* writeData, uint8_t bytesToWrite, uint8_t bytesToRead)
{
	// Buffergröße überprüfen
	if(bytesToWrite > TWIM_WRITE_BUFFER_SIZE)
		return false;
	if(bytesToRead > TWIM_READ_BUFFER_SIZE)
		return false;

	// Transaktion starten, falls Bus bereit
	if(twi->status == TWIM_STATUS_READY) {
		// Buszustand ändern
		twi->status = TWIM_STATUS_BUSY;
		twi->result = TWIM_RESULT_UNKNOWN;

		// Datenbuffer füllen
		for(uint8_t bufferIndex=0; bufferIndex<bytesToWrite; ++bufferIndex) {
			twi->writeData[bufferIndex] = writeData[bufferIndex];
		}

		// TWI Variablen nullen
		twi->address = address;
		twi->bytesToWrite = bytesToWrite;
		twi->bytesToRead = bytesToRead;
		twi->bytesWritten = 0;
		twi->bytesRead = 0;

		// TWI-Master Transaktion
 		if(twi->bytesToWrite > 0) { // write
			uint8_t writeAddress = twi->address & ~0x01;
			twi->interface->MASTER.ADDR = writeAddress;
		}
		else if(twi->bytesToRead > 0) { // read
			uint8_t readAddress = twi->address | 0x01;
			twi->interface->MASTER.ADDR = readAddress;
		}
		
		// Erfolgreich
		return true;
	}
	else {
		// Bus beschäftigt
		return false;
	}
}

// TWI-Master Transaktion durchführen
bool TWI_MasterWriteNull(TWI_Master_t* twi, uint8_t address)
{
	// Transaktion starten, falls Bus bereit
	if(twi->status == TWIM_STATUS_READY) {
		// Buszustand ändern
		twi->status = TWIM_STATUS_BUSY;
		twi->result = TWIM_RESULT_UNKNOWN;

		// TWI Variablen nullen
		twi->address = address;
		twi->bytesToWrite = 0;
		twi->bytesToRead = 0;
		twi->bytesWritten = 0;
		twi->bytesRead = 0;

		// TWI-Master Transaktion
		uint8_t writeAddress = twi->address & ~0x01;
		twi->interface->MASTER.ADDR = writeAddress;
		
		// Erfolgreich
		return true;
	}
	else {
		// Bus beschäftigt
		return false;
	}
}

// TWI-Interrupthandler
void TWI_MasterInterruptHandler(TWI_Master_t* twi)
{
	// Zustand
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	// Interrupttyp
	if ((currentStatus & TWI_MASTER_ARBLOST_bm) || (currentStatus & TWI_MASTER_BUSERR_bm)) { // Fehler
		TWI_MasterArbitrationLostBusErrorHandler(twi);
	}
	else if(currentStatus & TWI_MASTER_WIF_bm) { // Writeinterrupt
		TWI_MasterWriteHandler(twi);
	}
	else if(currentStatus & TWI_MASTER_RIF_bm) { // Readinterrupt
		TWI_MasterReadHandler(twi);
	}
	else { // anderer Zustand
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_FAIL);
	}
}

// Fehler-Handler
void TWI_MasterArbitrationLostBusErrorHandler(TWI_Master_t *twi)
{
	// Zustand
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	// Fehlertyp
	if(currentStatus & TWI_MASTER_BUSERR_bm) { // Busfehler
		twi->result = TWIM_RESULT_BUS_ERROR;
	}
	else { // Lostfehler
		twi->result = TWIM_RESULT_ARBITRATION_LOST;
	}

	// TWI Transaktion beenden
	twi->interface->MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;
	twi->status = TWIM_STATUS_READY;
}

// Writeinterrupt
void TWI_MasterWriteHandler(TWI_Master_t* twi)
{
	// Variablen
	uint8_t bytesToWrite = twi->bytesToWrite;
	uint8_t bytesToRead = twi->bytesToRead;

	// Zustände
	if(twi->interface->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		// Abbrechen wenn Slave-ACK fehlt (NACK)
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi->result = TWIM_RESULT_NACK_RECEIVED;
		twi->status = TWIM_STATUS_READY;
	}
	else if(twi->bytesWritten < bytesToWrite) {
		// weiteres Byte senden
		uint8_t data = twi->writeData[twi->bytesWritten];
		twi->interface->MASTER.DATA = data;
		++twi->bytesWritten;
	}
	else if (twi->bytesRead < bytesToRead) {
		// fertig geschrieben, aber noch Daten zum Lesen vorhanden -> rep_start
		uint8_t readAddress = twi->address | 0x01;
		twi->interface->MASTER.ADDR = readAddress;
	}
	else {
		// Transaktion beendet -> stop
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}

// Readinterrupt
void TWI_MasterReadHandler(TWI_Master_t* twi)
{
	// Daten in Buffer lesen
	if(twi->bytesRead < TWIM_READ_BUFFER_SIZE) {
		uint8_t data = twi->interface->MASTER.DATA;
		twi->readData[twi->bytesRead] = data;	
		++twi->bytesRead;
	}
	else {
		// Buffer Overflow
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_BUFFER_OVERFLOW);
	}

	// Variablen
	uint8_t bytesToRead = twi->bytesToRead;

	// Lesezustand
	if(twi->bytesRead < bytesToRead) {
		// weiteres Byte lesen ACK
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
	}
	else {
		// letztes Byte gelesen NACK -> stop
		twi->interface->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}

// TWI Transaktion beenden
void TWI_MasterTransactionFinished(TWI_Master_t* twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIM_STATUS_READY;
	
	
}
