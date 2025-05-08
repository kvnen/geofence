#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> //delete later!!! need to make timebase etc...
#include "nmea.h"
#include "utils.h"
#include "uart.h"
#include "LCD.h"

volatile uint16_t geofence_violation = 0;
volatile uint16_t led_toggle_flag = 0;

void timer_for_led(void) {
	TCCR2A = (1 <<WGM21);
	TCCR2B = (1 << CS22) | (1 <<CS20);
	OCR2A = 24999;
	TIMSK2 |= (1<<OCIE2A);
}

ISR(TIMER2_COMPA_vect){
	if (geofence_violation){
		if (led_toggle_flag == 0)
		{
		
		PORTD |=(1<<PORTD4);
		PORTD &= ~(1<<PORTD5);
		led_toggle_flag = 1;
		} else {
		
		PORTD &= ~(1<<PORTD4);
		PORTD |= (1<<PORTD5);
		led_toggle_flag = 0;
		}
	} else {
		PORTD &= ~(1<<PORTD4);
		PORTD &= ~(1<<PORTD5);
		led_toggle_flag =0;
	}
}

int main(void){
	// LCD!!
	
	TWI_init();
	LCD_init();
	
// ^LCD!
	
	ADCint(); // read potentiometer
	LEdint(); //set ledpins

	PWMint(); //buzzer with pwm
	buttoninit();
	initUART0(UBRR); //initializing the uarts
	initUART1(UBRR);
	volatile float home_lat = 0.0;
	volatile float home_lon = 0.0;

	sei();

	uint8_t home_set =0;

	enableUART1Tx();

	for(unsigned char i = 0; i < 10; i++){
		char pmtk_cmd[] = "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
		_delay_ms(100);
		writeUART(getUART1TxBuffer(), pmtk_cmd,51);
		enableUART1Tx();
	}


	posdata location;
	while (1) 
	{
		location = parseNMEA(getUART1RxBuffer());

		writeUART(getUART0TxBuffer(), getUART1RxBuffer(), 82);
		enableUART0Tx();

		LCD_setCursor(0, 0);
		LCD_print(location.lats);
		LCD_setCursor(0, 1);
		LCD_print(location.lons);

		if(button_pressed() && !home_set){
			LCD_setCursor(13, 1);
			LCD_print("set");

			uint16_t radius = geofence_radius();

			home_lat = location.lat;
			home_lon = location.lon;

			area_save(location.lat,location.lon , radius);
			home_set = 1;
		}

		uint16_t radius = geofence_radius();
		uint16_t distance = distance_calculation(home_lat, home_lon, location.lat, location.lon); 

		if (distance > radius){
			geofence_violation = 1;
			buzzeron();
		} else{
			geofence_violation = 0;
			buzzeroff(); //buzzer off
		}	
	}
}

