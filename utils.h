#ifndef UTILS_H
#define UTILS_H

#include <avr/io.h>

#ifndef MAXRANGE
#define MAXRANGE 50
#endif
#ifndef MINRANGE
#define MINRANGE 5
#endif
// ADC convert (pontentiometer)
void ADCint(void);
uint16_t ADCreading(void);  //Reads from PC0
uint16_t geofence_radius(void);

//LEDS
void policeLightsOn();
void policeLightsOff();
void timer_for_led(void);
void LEdint(void);
void LEDred_on(void);
void LEdred_off(void);
void LEDblue_on(void);
void LEDblue_off(void);
void LEDblink(void); //changing blue/red

//buzzer and PWM
void PWMint(void);
void buzzeron(unsigned char input);
void buzzeroff(void);

//button things
void buttoninit(void);
uint8_t button_pressed(void);

//setting "home" and checking distance
float distance_calculation(float lat1, float lon1, float lat2, float lon2);
#endif

