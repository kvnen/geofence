#include <stdlib.h>
#include "nmea.h"

char *tokenPtrs[15];
void tokenize(char *input, char *tokenPtrs[], unsigned char maxTokens){
	unsigned char tokens = 0;
	tokenPtrs[tokens++] = input;
	for(unsigned char i = 0; tokens < maxTokens && input[i] != '*'; i++){
		if(input[i] == ','){
			input[i] = '\0';
			tokenPtrs[tokens++] = &input[i + 1];
		}
	}
}

posdata parseNMEA(char *input) {
	tokenize(input, tokenPtrs, 15);
	posdata output;
	output.lat = atof(tokenPtrs[2]);
	output.lon = atof(tokenPtrs[4]);
	output.fix = atoi(tokenPtrs[6]);
	output.satellites = atoi(tokenPtrs[7]);
	for(unsigned char i = 0; i < 9; i++){
		output.lats[i] = tokenPtrs[2][i];
	}
	for(unsigned char i = 0; i < 10; i++){
		output.lons[i] = tokenPtrs[4][i];
	}
	return output;
}

