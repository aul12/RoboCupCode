/*
 * debug.h
 *
 * Created: 21.04.2016 11:58:37
 *  Author: rag
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

inline void sendData(uint8_t addr, uint8_t data){
	usart_putc(&debug, '|');
	usart_putc(&debug, addr);
	usart_putc(&debug, data);
	usart_putc(&debug, '&');
	wdt_delay(10);
}



#endif /* DEBUG_H_ */