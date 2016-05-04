extern "C" {
	#include <avr/interrupt.h>	// ISR - Interrupt
}
#include "../driver/usart.h"	// UART Kommunikation
#include "../defines.h"			// globale Definitionen

extern volatile int16_t phi_jetzt;
extern volatile int16_t tor_winkel;
extern volatile int16_t tor_winkel_rel;
extern volatile int16_t _xdiff;

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


volatile uint8_t pixyNoObjectTimer = 0;

//Pixy-Vorne
USART_data_t pixyVorneUART;

uint8_t muxPixy1 = 0;

volatile PixyResult_t *pixyVorneData, *pixyVorneReceiveData;

volatile uint8_t newPixyData = 0;

volatile uint8_t newPixyDiff = 0;

void pixyInit()
{
	usart_init(&pixyVorneUART, &USARTC0, USART_BAUD_SELECT(9600, F_CPU), USART_DREINTLVL_LO_gc, USART_RXCINTLVL_MED_gc, true);
}

ISR(USARTC0_RXC_vect)
{
	// Daten empfangen
	uint8_t data = pixyVorneUART.usart->DATA;
		
	switch(muxPixy1){
		case 0:
			if(data != 0x55){
				muxPixy1 = 0;
				return;
			}
			break;
		case 1:
		case 3:
			if(data != 0xaa){
				muxPixy1 = 0;
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
				muxPixy1 = 0;
				return;
			}
			
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
				
				//tor_winkel = ((atan(-x_diff*0.0047957937)*57.3 + phi_jetzt)+tor_winkel*9)/10;
				tor_winkel = ((-x_diff/4 + phi_jetzt)+tor_winkel*9)/10;
				
				pixyNoObjectTimer = 0;
			}	
			else{
				pixyVorneReceiveData->checksumStimmt = false;
			}
						
				
			
			muxPixy1 = 0;
			return;
			
	}
	
	muxPixy1++;
}

