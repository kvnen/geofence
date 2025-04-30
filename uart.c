#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>


ISR(USART1_RX_vect){
	UDR0 = UDR1;		
}

void initUART1(unsigned short ubrr){	//setup for uart receiving from gps
	UBRR1H = (unsigned char)(ubrr>>8);	//set the higher UBRR
	UBRR1L = (unsigned char) ubrr;		//set the lower UBRR
	UCSR1B = (1<<TXEN1)|(1<<RXEN1)|(1<<RXCIE1);	//enable transmit, receive and receive interrupt
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);	//set message to 8 bits
}

void initUART0(unsigned short ubrr){	//setup for usb serial to pc
	UBRR0H = (unsigned char)(ubrr>>8);	//set the higher UBRR
	UBRR0L = (unsigned char) ubrr;		//set the lower UBRR
	UCSR0B = (1<<TXEN0);			//enable transmit
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);	//set message to 8 bits
}



