#include "nmea.h"
#include "uart.h"

#include <stdlib.h>

pos parseNMEA(char *input){
	pos output;
	if(readBuffer(getUART1RxBuffer())=='$'){
		while(readBuffer(getUART1RxBuffer())!=','); //GPGGA
		if(readBuffer(getUART1RxBuffer())==','){ //detect if no pos fix and return 0
			output.satellites = 0;
			output.lat = 0;
			output.lon = 0;
			return output;
		}
		char lat[9];
		for(unsigned char i = 0; i < 9; i++){ //0000.0000
			lat[i] = readBuffer(getUART1RxBuffer());
		}
		output.lat = atof(lat);
		for(unsigned char i = 1; i <= 3; i++){ //,N,
		readBuffer(getUART1RxBuffer());
		}
		char lon[10];
		for(unsigned char i = 0; i < 10; i++){ //00000.0000
			lon[i] = readBuffer(getUART1RxBuffer());
		}
		output.lon = atof(lon);
		for(unsigned char i = 1; i <= 3; i++){ //,N,
		readBuffer(getUART1RxBuffer());
		}
		output.satellites = readBuffer(getUART1RxBuffer()); 
	}	
	return output;
}
