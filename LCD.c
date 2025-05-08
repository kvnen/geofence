#include "LCD.h"
#include <avr/io.h>
#include <util/delay.h> //libraries
// mostly from slides. SLightly edited. Code is split into 3 pieces to not clutter main
// ´this file is mostly the logic for the LCD and printing text into it
//initialize twi i2c commnication
void TWI_init(void){
	TWSR0 = 0x00; // prescaler to 1 (bits to 0)
	TWBR0 = ((F_CPU / SCL_CLOCK) - 16) / 2; // Bit rate register for wanted scl frequency
}

//send a stop condition on the i2c bus
void TWI_stop(void){
	TWCR0 = (1<<TWSTO) | (1<<TWEN) | (1<<TWINT); // Stop condition, clear twi ánd clear interrupt flag
	_delay_ms(1); //delay. prbobles with stop conditoin otherwise. DO NOT REMOVE!!!
}

//send a start condition to i2c bus
void TWI_start(void){
	TWCR0 = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // start condition. enable twi clear interrupt flag
	while (!(TWCR0 & (1 << TWINT))); // wait for twint flag set. tart condition transmnitted
}

//write a byte of data to the i2c bus
void TWI_write(uint8_t data){
	TWDR0 = data; // load data into TWI data register
	TWCR0 = (1<<TWEN) | (1<<TWINT); // enable TWI, clear interrupt flag to start transmission
	while (!(TWCR0 & (1<<TWINT))); // wait for transmission to complete
}

// send a command byte to the LCD over i2c
void LCD_sendCommand(uint8_t cmd) {
	TWI_start(); // start lcd communnication (i2c)
	TWI_write(LCD_ADDR << 1); // send address with write bit 0
	TWI_write(0x80); // Co = 1, RS = 0 (Command Mode)
	TWI_write(cmd); //sendin command yeeet
	TWI_stop();// end 
}

//send adata byte char to the lcd over i2c. 
void LCD_sendData(uint8_t data) {
	TWI_start(); // start
	TWI_write(LCD_ADDR << 1); // Write address
	TWI_write(0x40); // Co = 0, RS = 1 (Data Mode)
	TWI_write(data); // chaarcter
	TWI_stop();
}

//borrowed mostly from slides
void LCD_init(void) {
	_delay_ms(50); // Wait for LCD power-up
	LCD_sendCommand(0x38); // 8-bit, 2-line, normal font
	LCD_sendCommand(0x39); // function set
	LCD_sendCommand(0x14); // fnternal OSC frequency
	LCD_sendCommand(0x70); // contrast set
	LCD_sendCommand(0x56); // power/icon/contrast control
	LCD_sendCommand(0x6C); // Follower control
	_delay_ms(200); // stablize voltage. 
	LCD_sendCommand(0x38); // Function set
	LCD_sendCommand(0x0C); // Display ON
	LCD_sendCommand(0x01); // Clear display
	_delay_ms(2);
}

//set the cursor postiion on lcd so the text is actually in readable format. 
void LCD_setCursor(uint8_t col, uint8_t row) {
	uint8_t address = (row == 0) ? col : (0x40 + col);
	LCD_sendCommand(0x80 | address);
}


// null
void LCD_print(const char *str) {
	while (*str) {
		LCD_sendData(*str++);
	}
}
