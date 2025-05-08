#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define NMEALENGTH 82 //standard length of an nmea message

buffer *UART0RxBuffer = NULL; //pointers for all the uart buffers
buffer *UART0TxBuffer = NULL;
char *UART1RxBuffer = NULL; //this buffer is what gets written on straight from UDR
char *UART1RxOutput = NULL; //this buffer is a finished nmea message
buffer *UART1TxBuffer = NULL;

//getter functions for the uart buffer pointers
buffer* getUART0RxBuffer(){
	return UART0RxBuffer;
}

char* getUART1RxBuffer(){
	return UART1RxOutput;
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
	UART1RxBuffer = malloc(sizeof(char)*NMEALENGTH); 
	UART1RxOutput = malloc(sizeof(char)*NMEALENGTH); 
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
	if(((wbuffer->head + 1) == (BUFFERSIZE - 1)) & wbuffer->tail){ //if the buffer is full it becomes empty (very philosophical)
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

void enableUART0Tx(){ //abstraction for uart tx enabling.
	if(UART0TxBuffer->head != UART0TxBuffer->tail){
	UCSR0B |= (1 << UDRIE0);
	}
}

void enableUART1Tx(){
	if(UART1TxBuffer->head != UART1TxBuffer->tail){
	UCSR1B |= (1 << UDRIE1);
	}
}

ISR(USART0_RX_vect){ //write to buffer from udr when receiving data.
	writeBuffer(UART0RxBuffer, UDR0);
}

volatile unsigned char count = 0;
ISR(USART1_RX_vect){
	if(UDR1 == '$'){ //$ starts the nmea message
		UART1RxBuffer[count++] = UDR1;
	}
	else if(UDR1 == '*'){ 	//the message ends at * which is not perfect and causes some funny stuff
		UART1RxBuffer[count] = UDR1;
		for(unsigned char i = 0; i < NMEALENGTH; i++){ //copying the message to output when its complete
			UART1RxOutput[i] = UART1RxBuffer[i];
		}
		count = 0;
	}
	else{
		UART1RxBuffer[count++] = UDR1;
	}
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
		UDR1 = readBuffer(UART1TxBuffer);
	}
	else{
		UCSR1B &= ~(1 << UDRIE1);
	}
}
