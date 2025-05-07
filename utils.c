#include "utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>


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
	return 5 + ((uint32_t)ADCvalue*(100 - 5)) /1023;
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
void LEDblink(void) {
	TCCR2A = (1 << WGM21); // put in ctc mode
	TCCR2B = (1 << CS22);  // prescaler 64
	OCR2A = 249; //250ticks = 1ms
	TIMSK2 |= (1<<OCIE2A); //compare match interrupt
	sei(); //global interrupts 
}

volatile uint16_t blinkcounter = 0;

ISR(TIMER2_COMPA_vect) {
	blinkcounter++;
	if (blinkcounter >= 100){ //100ms delay
		blinkcounter = 0;
		PORTD ^= (1<<PORTD4);  //red led
		PORTD ^= (1<<PORTD5);   //blue led
	}
}

//PWM setup
void PWMint(void){
	DDRD |= (1<<PORTD6);
	TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00); //fast pwm
	TCCR0B = (1<<CS01); //prescaler 8
}	
void buzzeron(void){
	OCR0A = 249; // 100% 
}
void buzzeroff(void){
	OCR0A = 0; // 0%
}