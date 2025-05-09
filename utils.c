#include "utils.h"
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>
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
	return 5 + ((uint32_t)ADCvalue*(MAXRANGE - MINRANGE)) /1023; // min and max range should be set in main
}

//Setting up all the led states

void LEdint(void){
	DDRD |= (1<<DDRD4) | (1<<DDRD6); //PD4 is red, PD5 is blue
}

void LEDred_on(void){
	PORTD |= (1<<PORTD4);
}

void LEdred_off(void){
	PORTD &= ~(1<<PORTD4);
}

void LEDblue_on(void){
	PORTD |= (1<<PORTD6);
}

void LEDblue_off(void){
	PORTD &= ~(1 << PORTD6);
}

//PWM setup
void PWMint(void){
	TCCR0A = (1<<COM0B1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00); //fast pwm top = ocra
	TCCR0B = (1<<CS00)|(1<<CS01); //prescaler 64
	OCR0A = 255;  //top value
	OCR0B = 124;  //this controls dutycycle
}	

void buzzeron(unsigned char value){ //turns on the buzzer and controls the frequency
	DDRD |= (1<<PORTD5); 
	OCR0A = value;
	OCR0B = value/2; // for now the dutycycle is always 50% idk what is good for buzzer
}

void buzzeroff(void){
	DDRD &= ~(1<<PORTD5);
}

//convert degrees to radians
float deg_to_rad(float deg) {
    return deg * (PI / 180.0f);
}

//NMEA format to decimal degrees
float nmea_to_decimal(float val) {
    int degrees = (int)(val / 100);
    float minutes = val - (degrees * 100);
    return degrees + (minutes / 60.0f);
}

//accepts NMEA-format coordinates and returns distance in meters
float distance_calculation(float raw_lat1, float raw_lon1, float raw_lat2, float raw_lon2) {
    float lat1 = nmea_to_decimal(raw_lat1);
    float lon1 = nmea_to_decimal(raw_lon1);
    float lat2 = nmea_to_decimal(raw_lat2);
    float lon2 = nmea_to_decimal(raw_lon2);

    float dLat = lat2 - lat1;
    float dLon = lon2 - lon1;

    float latDist = dLat * DEGTOMETER;
    float avgLat = (lat1 + lat2) / 2.0f;
    float lonDist = dLon * DEGTOMETER * cosf(deg_to_rad(avgLat));

    return sqrtf(latDist * latDist + lonDist * lonDist);
}

void buttoninit(void) {
	DDRD &= ~(1 << DDD2);    // Set PD2 as input
	PORTD |= (1 << PORTD2);  // Enable pull-up resistor
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
	buzzeroff();
	TIMSK2 &= ~(1<<OCIE2A);
	PORTD &= ~((1<<PORTD6)|(1<<PORTD4));
}

volatile unsigned char tick_count = 0; 

ISR(TIMER2_COMPA_vect){ //toggles the lights every 1s
	tick_count++;
	if (tick_count > 64){
		PORTD ^= (1<<PORTD6)|(1<<PORTD4);
		tick_count = 0;
		if(OCR0A == 255){ //also added the buzzer here for alternating tones and stuff
			buzzeron(100);
		}
		else{
			buzzeron(255);
		}
	}
}






void buttoninit(void) [{
DDRD &= ~(1<<DDRD2);
PORTD |=(1<<PORTD2);
PCICR |= (1<<PCIE1); //enable pin change interrupt group
PCMSK2 |= (1<<PCINT18); // enable pcint2
sei(); //global interrupts
}

volatile unsigned char state = 0;

ISR(PCINT2_vect) {
	unsigned char last_state = 1;
	unsigned char current_state = !(PIND & (1 << PIND2));
	if (current_state && !last_state) {
		_delay_ms(10); // debounce delay
	
	if (!(PIND & (1 << PIND2))) {
		state = 1;
	}
}
	last_state = current_state;
	}
	
uint8_t button_pressed(void) {
	if (state) {
		state = 0;
		return 1;
	}
	return 0;
}
		
	


