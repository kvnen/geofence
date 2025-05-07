#ifndef NMEA_H
#define NMEA_H
//CODE HERE
#include "uart.h"
typedef struct {
	float lat;
	float lon;
	int fix;
	int satellites;
} posdata;
posdata parseNMEA(char *input);
#endif
