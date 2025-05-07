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

	const char* text1 = "moi";
	const char* text2 = "heihei";
	const char* text3 = "olele";
	const char* text4 = "olala";

	LCD_setCursor(0, 0);
	LCD_print(text1);

	LCD_setCursor(0, 1);
	LCD_print(text2);
	
	_delay_ms(200);
	
	LCD_setCursor(0, 0);
	LCD_print(text3);

	LCD_setCursor(0, 1);
	LCD_print(text4);
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

	uint16_t home_set =0;

	while (1) 
	{
		posdata currentLocation = parseNMEA(getUART1RxBuffer());
		if(currentLocation.lat > 1){
			writeUART(getUART0TxBuffer(), "fix???\r\n", 8);
			enableUART0Tx();
		}

		if(button_pressed() && !home_set){
			uint16_t radius = geofence_radius();

			home_lat = currentLocation.lat;
			home_lon = currentLocation.lon;

			area_save(currentLocation.lat,currentLocation.lon , radius);
			home_set = 1;
		}
		uint16_t radius = geofence_radius();
		uint16_t distance = distance_calculation(home_lat, home_lon, currentLocation.lat, currentLocation.lon); 

		if (distance > radius){
			geofence_violation = 1;
			buzzeron();
		} else{
			geofence_violation = 0;
			buzzeroff(); //buzzer off
		}	
	}
}

