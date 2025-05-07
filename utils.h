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



//setting "home" and checking distance
void area_save(float lat, float lon, uint16_t radius);
float distance_calculation(float lat1, float lon1, float lat2, float lon2);
uint16_t geofence_check(float current_lat, float current_lon);
#endif

void buttoninit(void);
uint8_t button_pressed(void);
