#ifndef NMEA_H
#define NMEA_H
typedef struct {
	float lat;
	char lats[9];
	float lon;
	char lons[10];
	int fix;
	int satellites;
} posdata;
posdata parseNMEA(char *input);
#endif
