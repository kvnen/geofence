#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define baudrate 9600;
void initUART1(void){
	UCSR1B = (1<<RXEN1)|(1<<RXCIE1);
}

