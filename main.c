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
	float home_lat = 0.0;
	float home_lon = 0.0;
	uint16_t radius = 0;
	unsigned short distance = 0;

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

		if(button_pressed() && !home_set){
			LCD_setCursor(13, 1);
			LCD_print("set");

			radius = geofence_radius();

			home_lat = location.lat;
			home_lon = location.lon;

			home_set = 1;
		}

		if(home_set){
			distance = distance_calculation(home_lat, home_lon, location.lat, location.lon); 
		}
		else{
			distance = 0;
		}

		if(geofence_radius() < 10){
			LCD_setCursor(0, 0);
			LCD_print("rad: 10");
		}
		else if(geofence_radius() < 20){
			LCD_setCursor(0, 0);
			LCD_print("rad: 20");
		}
		else if(geofence_radius() < 30){
			LCD_setCursor(0, 0);
			LCD_print("rad: 30");
		}
		else if(geofence_radius() < 40){
			LCD_setCursor(0, 0);
			LCD_print("rad: 40");
		}
		else if(geofence_radius() < 50){
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
			policeLightsOn();
		} else{
			buzzeroff(); //buzzer off
			policeLightsOff();
		}	
	}
}
