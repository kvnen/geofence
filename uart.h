#ifndef UART_H
#define UART_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUDRATE 9600
#define UBRR (F_CPU/16/BAUDRATE-1)

#define BUFFERSIZE 128 // has to be power of 2 eg 2^7 for blazingly fast wrapping but less than 255
		       //
typedef struct { //struct for buffer: head writes, tail reads
	unsigned char data[BUFFERSIZE];
	unsigned char head;
	unsigned char tail;
} buffer;

void initUART1(unsigned short ubrr);
void initUART0(unsigned short ubrr);
void enableUART0Tx();
void enableUART1Tx();
void writeUART(buffer *wbuffer, char input[], unsigned char size);
char readBuffer(buffer *rbuffer);
void writeBuffer(buffer *wbuffer, char input);
buffer* getUART0RxBuffer();
buffer* getUART1RxBuffer();
buffer* getUART0TxBuffer();
buffer* getUART1TxBuffer();
			

#endif
