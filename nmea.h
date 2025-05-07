#ifndef NMEA_H
#define NMEA_H
//CODE HERE
#include "uart.h"
typedef struct {
	unsigned short lat;
	unsigned short lon;
	unsigned short height;
	int fix;
	int satellites;
} posdata;
posdata parseNMEA(buffer *buf);
#endif
