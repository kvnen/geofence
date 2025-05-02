#ifndef UART_H
#define UART_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUDRATE 9600
#define UBRR (F_CPU/16/BAUDRATE-1)

void initUART1(unsigned short ubrr);
void initUART0(unsigned short ubrr);

#endif
