#include <stdlib.h>
#include "nmea.h"

float parseFloat(char **input) {
    char buffer[16] = {0};
    int i = 0;

    while (**input != ',' && **input != '\0' && i < sizeof(buffer) - 1) {
        buffer[i++] = **input;
        (*input)++;
    }

    if (**input == ',') {
        (*input)++;
    }

    return atof(buffer);
}

int parseInt(char **input) {
    char buffer[8] = {0};
    int i = 0;

    while (**input != ',' && **input != '\0' && i < sizeof(buffer) - 1) {
        buffer[i++] = **input;
        (*input)++;
    }

    if (**input == ',') {
        (*input)++;
    }

    return atoi(buffer);
}

posdata parseNMEA(char *input) {
    posdata output = {0};

    // Skip first 2 fields: "$GPGGA", "time"
    for (int i = 0; i < 2; i++) {
        while (*input != ',' && *input != '\0') input++;
        if (*input == ',') input++;
    }

    // If latitude is empty, return default data
    if (*input == ',') {
        return output;
    }

    // Latitude
    output.lat = parseFloat(&input);

    // N/S
    if (*input != ',' && *input != '\0') input++;
    if (*input == ',') input++;

    // Longitude
    output.lon = parseFloat(&input);

    // E/W
    if (*input != ',' && *input != '\0') input++;
    if (*input == ',') input++;

    // Fix status
    output.fix = parseInt(&input);

    // Satellites
    output.satellites = parseInt(&input);

    return output;
}

