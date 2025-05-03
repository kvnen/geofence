#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> //delete later!!! need to make timebase etc...
#include "nmea.h"
#include "utils.h"
#include "uart.h"

int main(void)
{
	initUART0(UBRR); //initializing the uarts
	initUART1(UBRR);

	sei();

	while (1) 
	{
		writeUART(getUART0TxBuffer(), "HelloWorld", 10);
		enableUART0Tx();
		_delay_ms(1000);
	}
}

