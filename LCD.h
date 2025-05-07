#ifndef LCD_H
#define LCD_H
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define SCL_CLOCK 100000L
#define LCD_ADDR 0x3E // LCD:n I2C-osoite

// TWI (I2C) funktiot
void TWI_init(void);
void TWI_start(void);
void TWI_write(uint8_t data);
void TWI_stop(void);

// LCD toiminnot
void LCD_init(void);
void LCD_sendCommand(uint8_t cmd);
void LCD_sendData(uint8_t data);
void LCD_setCursor(uint8_t col, uint8_t row);
void LCD_print(const char *str);

#endif
