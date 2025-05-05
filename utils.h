#ifndef UTILS_H
#define UTILS_H

#include <avr/io.h>

// ADC convert (pontentiometer)
void ADCint(void);
uint16_t ADCreading(void);  //Reads from PC0
uint16_t geofence_radius(void);

//LEDS
void LEdint(void);
void LEDred_on(void);
void LEdred_off(void);
void LEDblue_on(void);
void LEDblue_off(void);
void LEDblink(void); //changing blue/red

//buzzer and PWM
void PWMint(void);
void buzzeron(void);
void buzzeroff(void);

#endif


