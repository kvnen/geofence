#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define UARTBUFFERSIZE 255 //can be at most 255 else need to move uartBufferLen to short

volatile char uartBuffer[UARTBUFFERSIZE]; //buffer takes stuff in and prints it out
volatile unsigned char uartBufferLen = 0; //tracks how many bytes there are in the buffer

ISR(USART1_RX_vect){
	uartBuffer[uartBufferLen] = UDR1; //put the incoming char into buffer
	uartBufferLen++;
	UCSR0B |= (1<<UDRIE1);
}

ISR(USART0_UDRE_vect){
	UDR0 = uartBuffer[0]; //read the first char or buffer to uart
	for(unsigned char i = 0; i < sizeof(uartBuffer)-1; i++){ //shift everything in the buffer "right" by 1 
		uartBuffer[i]=uartBuffer[i+1];
	}
	uartBufferLen--;
	if (uartBufferLen == 0){
	UCSR0B &= ~(1<<UDRIE1);
	}
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
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);			//enable transmit
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);	//set message to 8 bits
}

void sendUART0(char input[]){
	if (uartBufferLen+sizeof(*input) < 255){
		return;
	}
	for(unsigned char i = 0; i < sizeof(*input); i++){
		uartBuffer[uartBufferLen+i] = input[i];
	}
}

