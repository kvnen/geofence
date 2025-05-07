#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> //delete later!!! need to make timebase etc...
#include "nmea.h"
#include "utils.h"
#include "uart.h"


uint16_t get_distance(void){ //!!!!!!!!!!!!!!!!!1 korvataan taa gps functiolla
	return 10;
}


int main(void)
{
	ADCint(); // read potentiometer
	LEdint(); //set ledpins
	LEDblink(); // blinking with leds
	PWMint(); //buzzer with pwm

	initUART0(UBRR); //initializing the uarts
	initUART1(UBRR);

	sei();

//	char pmtk_cmd[] = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
//
//	writeUART(getUART1TxBuffer(), pmtk_cmd,51);
//	enableUART1Tx();

	while (1) 
	{
		_delay_ms(1500);
		writeUART(getUART0TxBuffer(), findNMEA(getUART1RxBuffer()), 77);
		enableUART0Tx();
		uint16_t radius = geofence_radius();
		uint16_t distance = get_distance(); //!!!!!!!!! korvataan taa gps logiikalla 
		
		if (distance > radius){
			buzzeron(); //buzzer turn on
		} else
		{
		buzzeroff(); //buzzer off
		}	
	}
}

