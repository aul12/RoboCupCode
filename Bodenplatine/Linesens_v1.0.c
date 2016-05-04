/*
 * Linesens_v1.c
 *
 * Created: 11.06.2014 23:36:12
 *  Author: Thomas Wodtko
 */ 

#define F_CPU 16000000
#include "stddefs.h"
#include <avr/interrupt.h>

volatile uint8_t Data_Out=0;
volatile uint8_t working=0;

volatile uint8_t really = 0;

ISR(SPI_STC_vect)
{
	SETPORT(SPI_PORT_B,SPI_CS_B);
		Data_Out=SPDR;
		
	

/*if(status[device] == 2) {
	ADC_Werte[(device + 1) * 8 - ADC_pin - 1] = SPDR;
	if(device == 3) {
		ADC_pin++;
		ADC_pin &= 7;
	}
	status[device] = 0;

}


// next device
device++;
device &= 3;*/

// SPI is free
working = 0;
}

ISR(TIMER0_OVF_vect) { //muss noch gesetzt werden

/*if(!PINSET(SPI_PORT_A, SPI_SSTRB_A)) status[0] = 1;
if(!PINSET(SPI_PORT_B, SPI_SSTRB_B)) status[1] = 1;
if(!PINSET(SPI_PORT_C, SPI_SSTRB_C)) status[2] = 1;
if(!PINSET(SPI_PORT_D, SPI_SSTRB_D)) status[3] = 1;

if(PINSET(SPI_PORT_A, SPI_SSTRB_A) && status[0] == 1) status[0] = 2;
if(PINSET(SPI_PORT_B, SPI_SSTRB_B) && status[1] == 1) status[0] = 2;
if(PINSET(SPI_PORT_C, SPI_SSTRB_C) && status[2] == 1) status[0] = 2;
if(PINSET(SPI_PORT_D, SPI_SSTRB_D) && status[3] == 1) status[0] = 2;

SETPORT(PORTB,1);*/

}

void select_chip(uint8_t n) {
	if(n == 0) CLEARPORT(SPI_PORT_A,SPI_CS_A);
	else if(n == 1) CLEARPORT(SPI_PORT_B,SPI_CS_B);
	else if(n == 2) CLEARPORT(SPI_PORT_C,SPI_CS_C);
	else if(n == 3) CLEARPORT(SPI_PORT_D,SPI_CS_D);
}
void deselect_chip(uint8_t n) {
	if(n == 0) SETPORT(SPI_PORT_A,SPI_CS_A);
	else if(n == 1) SETPORT(SPI_PORT_B,SPI_CS_B);
	else if(n == 2) SETPORT(SPI_PORT_C,SPI_CS_C);
	else if(n == 3) SETPORT(SPI_PORT_D,SPI_CS_D);
}

/*uint8_t sendData(uint8_t data){
	if (!(working)){
		working=1;
		SPDR=data;
		return 1;
	}
	else return 0;
}*/
void sendData(uint8_t data){
	if (!(working)){
		working=1;
		CLEARPORT(SPI_PORT_B,SPI_CS_B);
		//_delay_us(1);
		SPDR=data;
		_delay_us(1);
	}
}

uint8_t DATA_to_start() {
	return 0;
}

int main(void)
{
	
TCCR0B |= (1<<CS01) | (1<<CS00);
TIMSK0 |= (1<<TOIE0);

	SPCR = (1<<SPIE)|(1<<SPE)|(1<<MSTR)|(0<<SPR1)|(1<<SPR0);
	SPSR = (1<<SPI2X);

DDRB = 0b101111;
DDRC = 0b111010;
DDRD |= 0b11010100;

PORTC |= (1<<1) | (1<<3);
PORTD |= (1<<4) | (1<<6);

sei();

//SETPORT(PORTB,0);
volatile uint8_t read=0;

for(int i=0;i<4;i++) {
	status[i] = 0;
}

    while(1)
    {	
		if(Data_Out > 0) {
			SETPORT(PORTB,0);	
		}
		else {
			//CLEARPORT(PORTB,0);	
		}
		
		/*if(Data_Out & (1<<5)) SETPORT(PORTB,0);	
		else CLEARPORT(PORTB,0);
		if(Data_Out & (1<<4)) SETPORT(PORTB,1);	
		else CLEARPORT(PORTB,1);*/
		
		/*if(!(working)) {
			if(status[device] == 0) {
				select_chip(device);
				sendData(DATA_to_start());
			}
			else if(status[device] == 2) {
				select_chip(device);
				sendData(0x00);
			}
		}*/
		/*
		if (read==0){
			if (!(working)){
				sendData(0b10001110);
				read=2;
			}
		}
		//if ((read==1)&&(!(PINSET(PINC,SPI_SSTRB_B)))) read=2;
		_delay_us(50);
		if ((read==2)&&((PINSET(PINC,SPI_SSTRB_B)))){
			 //_delay_us(10);
			 sendData(0);
			 read=0;
		}
		_delay_us(1);*/
		//PORTL=Data_Out&0b00111111;
		
		//PINB |= (1<<1);
		
		
		sendData(0b10001110);
		_delay_us(1000);
		sendData(0);
		_delay_us(1000);
		
		if(Data_Out > 32)
			PORTD |= (1 << 7);
		else
			PORTD &= ~(1 << 7);
			
		//_delay_ms(5000);
       
    }
}