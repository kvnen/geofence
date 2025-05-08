#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "nmea.h"
#include "utils.h"
#include "uart.h"
#include "LCD.h"

int main(void){
	// LCD!!
	
	TWI_init();
	LCD_init();
	
// ^LCD!
	
	ADCint(); // read potentiometer
	LEdint(); //set ledpins
	timer_for_led();
	PWMint(); //buzzer with pwm
	buttoninit();
	initUART0(UBRR); //initializing the uarts
	initUART1(UBRR);
	volatile float home_lat = 0.0;
	volatile float home_lon = 0.0;

	sei();

	uint8_t home_set = 0;

	enableUART1Tx();
	for(unsigned char i = 0; i < 10; i++){ //this sends the PMTK to the gps module many times so that it understands.
		char pmtk_cmd[] = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
		_delay_ms(100);
		writeUART(getUART1TxBuffer(), pmtk_cmd,51);
		enableUART1Tx();
	}


	posdata location;
	while (1) 
	{
		location = parseNMEA(getUART1RxBuffer());
		
//		LCD_setCursor(0,0);
//		LCD_print("          ");
//		LCD_setCursor(0,1);
//		LCD_print("          ");

//		writeUART(getUART0TxBuffer(), getUART1RxBuffer(), 82);
//		enableUART0Tx();

//		LCD_setCursor(0, 0);
//		LCD_print(location.lats);
//		LCD_setCursor(0, 1);
//		LCD_print(location.lons);

		policeLightsOn();

		uint16_t radius = 0;
		if(button_pressed() && !home_set){
			LCD_setCursor(13, 1);
			LCD_print("set");

			radius = geofence_radius();

			home_lat = location.lat;
			home_lon = location.lon;

			area_save(location.lat,location.lon , radius);
			home_set = 1;
		}

		uint16_t distance = distance_calculation(home_lat, home_lon, location.lat, location.lon); 

		if(radius < 10){
			LCD_setCursor(0, 0);
			LCD_print("rad: 10");
		}
		else if(radius < 20){
			LCD_setCursor(0, 0);
			LCD_print("rad: 20");
		}
		else if(radius < 30){
			LCD_setCursor(0, 0);
			LCD_print("rad: 30");
		}
		else if(radius < 40){
			LCD_setCursor(0, 0);
			LCD_print("rad: 40");
		}
		else if(radius < 50){
			LCD_setCursor(0, 0);
			LCD_print("rad: 50");
		}



		if(distance < 10){
			LCD_setCursor(0, 1);
			LCD_print("dist: 10");
		}
		else if(distance < 20){
			LCD_setCursor(0, 1);
			LCD_print("dist: 20");
		}
		else if(distance < 30){
			LCD_setCursor(0, 1);
			LCD_print("dist: 30");
		}
		else if(distance < 40){
			LCD_setCursor(0, 1);
			LCD_print("dist: 40");
		}
		else if(distance < 50){
			LCD_setCursor(0, 1);
			LCD_print("dist: 50");
		}

		if (distance > radius){
			buzzeron();
		} else{
			buzzeroff(); //buzzer off
		}	
	}
}
