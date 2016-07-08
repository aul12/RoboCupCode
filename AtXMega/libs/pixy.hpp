extern "C" {
	#include <avr/interrupt.h>	// ISR - Interrupt
}
#include "../driver/usart.h"	// UART Kommunikation
#include "../defines.h"			// globale Definitionen

extern volatile int16_t phi_jetzt;
extern volatile int16_t tor_winkel;
extern volatile int16_t tor_winkel_rel;
extern volatile int16_t _xdiff;
extern volatile int16_t ballOrangeWinkel;

typedef struct PixyResult{
	uint16_t breite;
	uint16_t hoehe;
	uint16_t xPos;
	uint16_t yPos;
	uint16_t id;
	uint8_t type;	
	uint16_t checksum;
	uint8_t checksumStimmt;
}PixyResult_t;

enum PixyType{
	NORMAL_OBJECT,
	COLOR_CODE_OBJECT
};

//Pixy-Vorne
USART_data_t pixyVorneUART, pixyHintenUART;

uint8_t muxPixyVorne = 0, muxPixyHinten = 0;

volatile PixyResult_t *pixyVorneData, *pixyVorneReceiveData, *pixyHintenData, *pixyHintenReceiveData;

volatile uint16_t noObjTimer = 0;

void pixyInit()
{
	usart_init(&pixyVorneUART, &USARTC0, USART_BAUD_SELECT(9600, F_CPU), USART_DREINTLVL_LO_gc, USART_RXCINTLVL_MED_gc, true);
	usart_init(&pixyHintenUART, &USARTD1, USART_BAUD_SELECT(9600, F_CPU), USART_DREINTLVL_LO_gc, USART_RXCINTLVL_MED_gc, true);
}

ISR(USARTC0_RXC_vect)
{
	// Daten empfangen
	uint8_t data = pixyVorneUART.usart->DATA;
		
	switch(muxPixyVorne){
		case 0:
			if(data != 0x55){
				muxPixyVorne = 0;
				return;
			}
			break;
		case 1:
		case 3:
			if(data != 0xaa){
				muxPixyVorne = 0;
				return;
			}
			break;
		case 2:
			if(data == 0x55){
				pixyVorneReceiveData->type = NORMAL_OBJECT;
			}
			else if(data == 0x56){
				pixyVorneReceiveData->type = COLOR_CODE_OBJECT;
			}	
			else{
				muxPixyVorne = 0;
				return;
			}
			break;
			
		case 4:
			pixyVorneReceiveData->checksum = data;
			break;
		case 5:
			pixyVorneReceiveData->checksum |= (data << 8);
			break;
		case 6:
			pixyVorneReceiveData->id = data;
			break;
		case 7:
			pixyVorneReceiveData->id |= (data << 8);
			break;
		case 8:
			pixyVorneReceiveData->xPos = data;
			break;
		case 9:
			pixyVorneReceiveData->xPos |= (data << 8);
			break;
		case 10:
			pixyVorneReceiveData->yPos = data;
			break;
		case 11:
			pixyVorneReceiveData->yPos |= (data << 8);
			break;
		case 12:
			pixyVorneReceiveData->breite = data;
			break;
		case 13:
			pixyVorneReceiveData->breite |= (data << 8);
			break;
		case 14:
			pixyVorneReceiveData->hoehe = data;
			break;
		case 15:
			pixyVorneReceiveData->hoehe |= (data << 8);
			
			
			
			if((pixyVorneReceiveData->breite + pixyVorneReceiveData->hoehe + pixyVorneReceiveData->xPos + pixyVorneReceiveData->yPos + pixyVorneReceiveData->id) == pixyVorneReceiveData->checksum){
				pixyVorneReceiveData->checksumStimmt = true;
				
				int16_t x_diff = 160-pixyVorneReceiveData->xPos;
				
				pixyVorneData = pixyVorneReceiveData;
				
				tor_winkel = ((-x_diff/4 + phi_jetzt)+tor_winkel*9)/10;
				
				noObjTimer = 0;
			}	
			else{
				pixyVorneReceiveData->checksumStimmt = false;
			}
						
				
			
			muxPixyVorne = 0;
			return;
			
	}
	
	muxPixyVorne++;
}


ISR(USARTD1_RXC_vect){
	// Daten empfangen
	uint8_t data = pixyHintenUART.usart->DATA;
	
	switch(muxPixyHinten){
		case 0:
			if(data != 0x55){
				muxPixyHinten = 0;
				return;
			}
		break;
		case 1:
		case 3:
			if(data != 0xaa){
				muxPixyHinten = 0;
				return;
			}
		break;
		case 2:
			if(data == 0x55){
				pixyHintenReceiveData->type = NORMAL_OBJECT;
			}
			else if(data == 0x56){
				pixyHintenReceiveData->type = COLOR_CODE_OBJECT;
			}
			else{
				muxPixyHinten = 0;
				return;
			}
			break;
		
		case 4:
			pixyHintenReceiveData->checksum = data;
		break;
		case 5:
			pixyHintenReceiveData->checksum |= (data << 8);
		break;
		case 6:
			pixyHintenReceiveData->id = data;
		break;
		case 7:
			pixyHintenReceiveData->id |= (data << 8);
		break;
		case 8:
			pixyHintenReceiveData->xPos = data;
		break;
		case 9:
			pixyHintenReceiveData->xPos |= (data << 8);
		break;
		case 10:
			pixyHintenReceiveData->yPos = data;
		break;
		case 11:
			pixyHintenReceiveData->yPos |= (data << 8);
		break;
		case 12:
			pixyHintenReceiveData->breite = data;
		break;
		case 13:
			pixyHintenReceiveData->breite |= (data << 8);
		break;
		case 14:
			pixyHintenReceiveData->hoehe = data;
		break;
		case 15:
			pixyHintenReceiveData->hoehe |= (data << 8);
		
		
		
			if((pixyHintenReceiveData->breite + pixyHintenReceiveData->hoehe + pixyHintenReceiveData->xPos + pixyHintenReceiveData->yPos + pixyHintenReceiveData->id) == pixyHintenReceiveData->checksum){
				pixyHintenReceiveData->checksumStimmt = true;
			
				int16_t x_diff = 160-pixyHintenReceiveData->xPos;
			
				ballOrangeWinkel = ((-x_diff/4 + phi_jetzt +180)+tor_winkel*9)/10;
				
				if(ballOrangeWinkel>180)
					ballOrangeWinkel -= 360;
				else if(ballOrangeWinkel<-180)
					ballOrangeWinkel += 360;
			}
			else{
				pixyHintenReceiveData->checksumStimmt = false;
			}
		
		
		
			muxPixyHinten = 0;
		return;
		
	}
	
	muxPixyHinten++;
}
