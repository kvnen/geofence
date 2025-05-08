#define F_CPU 16000000UL
#include <util/delay.h>
#include "utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#define DEGTOMETER 111320.0 //approx lenght in meters per degree
#define PI 3.14159265f 

void ADCint(void){
	ADMUX = (1<<REFS0); 
	ADCSRA = (1<<ADEN) | (1<<ADPS2); //We enable prescaler 16 to get fast readings and it has enough accuracy 
}

uint16_t ADCreading(void){
	ADCSRA |= (1<<ADSC); //start ADC conversion
	while (ADCSRA & (1 << ADSC)); //wait for conversion to be complete
	return ADC;   //return ADC
}
	
uint16_t geofence_radius(void){
	uint16_t ADCvalue = ADCreading(); //potentiometer on PC0
	return 5 + ((uint32_t)ADCvalue*(50 - 5)) /1023;
	//This should scale 0-1023 to our wanted range (for now) 5-100meters
}

//Setting up all the led states

void LEdint(void){
	DDRD |= (1<<DDRD4) | (1<<DDRD5); //PD4 is red, PD5 is blue
}

void LEDred_on(void){
	PORTD |= (1<<PORTD4);
}

void LEdred_off(void){
	PORTD &= ~(1<<PORTD4);
}

void LEDblue_on(void){
	PORTD |= (1<<PORTD5);
}

void LEDblue_off(void){
	PORTD &= ~(1 << PORTD5);
}

//blinking with the leds using timer2



//PWM setup
void PWMint(void){
	TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00); //fast pwm
	TCCR0B = (1<<CS00)|(1<<CS01); //prescaler 8
	OCR0A = 124; // 100% 
}	
void buzzeron(void){
	DDRD |= (1<<PORTD6);
}
void buzzeroff(void){
	DDRD &= ~(1<<PORTD6);
}




float home_lat = 0.0;
float home_lon = 0.0;
uint16_t home_radius = 0;

void area_save(float lat, float lon, uint16_t radius){
	home_lat = lat;
	home_lon = lon;
	home_radius = radius;
}

// Convert degrees to radians
float deg_to_rad(float deg) {
    return deg * (PI / 180.0f);
}

// Flat Earth distance approximation using floats
float distance_calculation(float lat1, float lon1, float lat2, float lon2) {
    // Coordinate differences
    float dLat = lat2 - lat1;
    float dLon = lon2 - lon1;

    // Convert latitude difference to meters
    float latDist = dLat * DEGTOMETER;

    // Use average latitude to convert longitude
    float avgLat = (lat1 + lat2) / 2.0f;
    float lonDist = dLon * DEGTOMETER * cosf(deg_to_rad(avgLat));

    // Pythagorean distance
    return sqrtf(latDist * latDist + lonDist * lonDist);
}

uint16_t geofence_check(float current_lat, float current_lon){
	float distance = distance_calculation(home_lat, home_lon, current_lat, current_lon);
	return(distance>home_radius);
}

void buttoninit(void) {
	DDRD &= ~(1 << DDD2);    // Set PD2 as input
	PORTD |= (1 << PORTD2);  // Enable pull-up resistor
}


uint8_t button_pressed(void) {
	static uint8_t last_state = 0;
	uint8_t current_state = !(PIND & (1 << PIND2));
	if (current_state && !last_state) {
		_delay_ms(10); // debounce delay
		if (!(PIND & (1 << PIND2))) {
			last_state = 1;
			return 1; // Press detected
		}
	}
	if (!current_state) {
		last_state = 0;
	}
	return 0;
}

void timer_for_led(void) { //setup for timer that blinks the leds
	TCCR2A |= (1 <<WGM21);
	TCCR2B |= (1 << CS22) | (1 <<CS20) | (1<<CS21);  //prescaler 1024
	OCR2A = 249;
}

void policeLightsOn(){ //checks if the lights are already on and then turns them on if not
	if(!(TIMSK2 & (1<<OCIE2A))){
		TIMSK2 |= (1<<OCIE2A); 
		PORTD |= (1<<PORTD4); //this is to offset the other led for easier toggling in ISR
	}
}

void policeLightsOff(){ //turns of the lights 
	TIMSK2 &= ~(1<<OCIE2A);
	PORTD &= ~(1<<PORTD5)|(1<<PORTD4);
}

volatile unsigned char tick_count = 0; 

ISR(TIMER2_COMPA_vect){ //toggles the lights every 1s
	tick_count++;
	if (tick_count > 64){
		PORTD ^= (1<<PORTD5)|(1<<PORTD4);
		tick_count = 0;
	}
}
