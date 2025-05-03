#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

buffer *UART0RxBuffer = NULL; //something about null pointers being unsafe or something
buffer *UART0TxBuffer = NULL;
buffer *UART1RxBuffer = NULL;
buffer *UART1TxBuffer = NULL;

buffer* getUART0RxBuffer(){
	return UART0RxBuffer;
}

buffer* getUART1RxBuffer(){
	return UART1RxBuffer;
}

buffer* getUART0TxBuffer(){
	return UART0TxBuffer;
}

buffer* getUART1TxBuffer(){
	return UART1TxBuffer;
}

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
	//if(((wbuffer->head + 1) & (BUFFERSIZE - 1)) & wbuffer->tail){ //if the buffer is full it becomes empty (very philosophical)
	//	wbuffer->tail = wbuffer->head; //this is done because otherwise when it would be read the old message would be cut off and mangled :(
	//}
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

void enableUART0Tx(){ //abstraction for uart tx enabling.
	UCSR0B |= (1 << UDRIE0);
}

void enableUART1Tx(){
	UCSR1B |= (1 << UDRIE1);
}

ISR(USART0_RX_vect){ //write to buffer from udr when receiving data.
	writeBuffer(UART0RxBuffer, UDR0);
}

ISR(USART1_RX_vect){
	writeBuffer(UART1RxBuffer, UDR1);
}

ISR(USART0_UDRE_vect){ //read from buffer till end and then disable UARTTX automatically when buffer runs empty
	if(UART0TxBuffer->tail != UART0TxBuffer->head){
		UDR0 = readBuffer(UART0TxBuffer);
	}
	else{
		UCSR0B &= ~(1 << UDRIE0);
	}
}

ISR(USART1_UDRE_vect){
	if(UART1TxBuffer->tail != UART1TxBuffer->head){
		UDR1 = UART1TxBuffer->data[UART1TxBuffer->tail];
	}
	else{
		UCSR1B &= ~(1 << UDRIE1);
	}
}
