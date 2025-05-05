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

//	char pmtk_cmd[] = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
//
//	writeUART(getUART1TxBuffer(), pmtk_cmd,51);
//	enableUART1Tx();

	while (1) 
	{
		unsigned char input = readBuffer(getUART1RxBuffer());
		if(input != 0xFF){
		writeBuffer(getUART0TxBuffer(), input);
		enableUART0Tx();
		}
	}
}

