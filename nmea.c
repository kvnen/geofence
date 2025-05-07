#include "nmea.h"
#include "uart.h"

#include <stdlib.h>

posdata parseNMEA(buffer *buf){
	while(readBuffer(buf) != '$'); //find start of nmea
	posdata output; //output var with default values. bad return can be detected with fix == 999
	output.fix = 999;
	output.satellites = 0;
	output.height = 0;
	output.lat = 0;
	output.lon = 0;
	char gpgga[] = "GPGGA";
	for(unsigned char i = 0; i < 5; i++){ //check that its GPGGA
		if(readBuffer(buf) == gpgga[i])	{
			continue;
		}
		else {
			return output;
		}
	}
	while(readBuffer(buf) != ','); //find the first ,
	for(unsigned char i = 0; i < 14; i++){ //there are 14 ',' in GPGGA
		
		switch (i){
			case 0: {
					while(readBuffer(buf) != ',') //skip over the utc segment
					break;
				}
			case 1: {
					char out[] = "0000000000";
					unsigned char a = 0;
					char b;
					while(1){
						b = readBuffer(buf);
						if (b == ',') {
							break;
						}
						if (b == '.') {
							continue;
						}
						out[i] = b;
						a++;
					}
					output.lat = atoi(out);
					break;
				}
			case 3: {
					char out[] = "0000000000";
					unsigned char a = 0;
					char b;
					while(1){
						b = readBuffer(buf);
						if (b == ',') {
							break;
						}
						if (b == '.') {
							continue;
						}
						out[i] = b;
						a++;
					}
					output.lon = atoi(out);
					break;

				}
			case 4: {
					output.fix = readBuffer(buf);
					while(readBuffer(buf) != ',');
					break;
				}
			case 5: {
					output.satellites = readBuffer(buf);
					while(readBuffer(buf) != ',');
					break;
			       }
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13: break;
		}
	}
	return output;
} 
