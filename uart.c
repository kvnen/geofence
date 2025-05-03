#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#define BUFFERSIZE 128 // has to be power of 2 eg 2^7 for blazingly fast wrapping but less than 255

typedef struct { //struct for buffer: head writes, tail reads
	char data[BUFFERSIZE];
	unsigned char head;
	unsigned char tail;
} buffer;

buffer *UART0RxBuffer = NULL; //something about null pointers being unsafe or something
buffer *UART0TxBuffer = NULL;
buffer *UART1RxBuffer = NULL;
buffer *UART1TxBuffer = NULL;

void initUART1(unsigned short ubrr){	//setup for uart receiving from gps
	UBRR1H = (unsigned char)(ubrr>>8);	//set the higher UBRR
	UBRR1L = (unsigned char) ubrr;		//set the lower UBRR
	UCSR1B = (1<<TXEN1)|(1<<RXEN1)|(1<<RXCIE1);	//enable transmit, receive and receive interrupt
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);	//set message to 8 bits
	UART1RxBuffer = malloc(sizeof(buffer)); 
	UART1TxBuffer = malloc(sizeof(buffer)); 
}

void initUART0(unsigned short ubrr){	//setup for usb serial to pc
	UBRR0H = (unsigned char)(ubrr>>8);	//set the higher UBRR
	UBRR0L = (unsigned char) ubrr;		//set the lower UBRR
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);			//enable transmit
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);	//set message to 8 bits
	UART0RxBuffer = malloc(sizeof(buffer)); 
	UART0TxBuffer = malloc(sizeof(buffer)); 
}

void writeBuffer(buffer *wbuffer, char input){
	if(wbuffer == NULL){ //check for nullptr
		return;
	}
	if(((wbuffer->head + 1) & (BUFFERSIZE - 1)) & wbuffer->tail){ //if the buffer is full it becomes empty (very philosophical)
		wbuffer->tail = wbuffer->head; //this is done because otherwise when it would be read the old message would be cut off and mangled :(
	}
	wbuffer->data[wbuffer->head] = input; //write to buffer and move the head	
	wbuffer->head = (wbuffer->head + 1) & (BUFFERSIZE - 1);
	return;
}

char readBuffer(buffer *rbuffer){
	if((rbuffer == NULL)||(rbuffer->tail == rbuffer->head)){ //check for nullptr and empty buffer
		return 0xFF; //return 0xFF if bad 	
	}
	char output = rbuffer->data[rbuffer->tail];
	rbuffer->tail = (rbuffer->tail + 1) & (BUFFERSIZE - 1);
	return output;
}

void writeUART(buffer *wbuffer, char input[], unsigned char size){
	for(unsigned char i = 0; i < size; i++){ // there is a possibility here of writing a message larger than the buffer but whatever.
		writeBuffer(wbuffer, input[i]);
	}	
}
