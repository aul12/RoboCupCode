
#ifndef STDDEFS_H_
#define STDDEFS_H_
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include <avr/pgmspace.h>

///////////// LEDs /////////////
#define PORT_LED PORTD
#define LED_RED 0
#define LED_YELLOW 1
#define LED_GREEN 2


/////////////// ADC-IC /////////////
//Values
uint8_t ADC_Werte[32];
// MAX1112 A
#define SPI_PORT_A PORTC
#define SPI_CS_A 3
#define SPI_SSTRB_A 2

// MAX1112 B
#define SPI_PORT_B PORTC
#define SPI_CS_B 1
#define SPI_SSTRB_B 0

// MAX1112 C
#define SPI_PORT_C PORTD
#define SPI_CS_C 6
#define SPI_SSTRB_C 5

// MAX1112 D
#define SPI_PORT_D PORTD
#define SPI_CS_D 4
#define SPI_SSTRB_D 3


// Status vars
//uint8_t working = 0;
uint8_t status[4];
uint8_t device = 0;
uint8_t ADC_pin = 0;




///////// FUNCTIONS ////////////
#define SETPORT(port,pin) (port|=(1<<pin))
#define CLEARPORT(port,pin) (port&=~(1<<pin))

#define SETLED(led) (PORT_LED|=(1<<led))
#define CLEARLED(led) (PORT_LED&=~(1<<led))
#define CHANGELED(led) (PIN_LED|=(1<<led))


#define TASTSET(tast) (!(PIN_TAST&(1<<tast)))
#define PINSET(port,pin) (port&(1<<pin))

#define nop() asm volatile("nop")

#endif /* STDDEFS_H_ */