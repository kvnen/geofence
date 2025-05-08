#ifndef LCD_H
#define LCD_H
#include <stdint.h>
#define F_CPU 16000000UL
#define SCL_CLOCK 100000L
#define LCD_ADDR 0x3E // LCD:n I2C-osoite

//code is mostly functions and macros. 

// TWI (I2C) funktiot
void TWI_init(void); //initialize hardware
void TWI_start(void); // sed start conditoin
void TWI_write(uint8_t data); // write byte od data to i2c 
void TWI_stop(void);  // send a stop condition

// LCD toiminnot - functions for the actual LCD
void LCD_init(void); // initialize the LCD itself. Settings, contrast, clear screen etc.
void LCD_sendCommand(uint8_t cmd); // send commadn to the ltc
void LCD_sendData(uint8_t data);´// sends data to be displayed
void LCD_setCursor(uint8_t col, uint8_t row); // setcursors by column and row
void LCD_print(const char *str); // prints the actual text on lcd. 

#endif
