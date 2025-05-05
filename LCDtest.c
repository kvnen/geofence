#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU16000000UL
#define SCL_CLOCK 100000L

#define  LCD_ADDR 0x3E //CHECK ATMEGA PCB FOR DDRESS!!!

void TWI_init(void){
	TWSR = 0x00; //prescaler to 1
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2; //bit rat register
}

void TWI_stop(void){
	TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);// stop
	_delay_ms(1);
}

void TWI_write(uint8_t data){
	TWDR = data;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}
void LCD_sendCommand(uint8_t cmd) {
	TWI_start();
	TWI_write(LCD_ADDR << 1); // Write address
	TWI_write(0x80); // Co = 1, RS = 0 (Command Mode)
	TWI_write(cmd);
	TWI_stop();
}

void LCD_sendData(uint8_t data) {
	TWI_start();
	TWI_write(LCD_ADDR << 1); // Write address
	TWI_write(0x40); // Co = 0, RS = 1 (Data Mode)
	TWI_write(data);
	TWI_stop();
}

void LCD_init(void) {
	_delay_ms(50); // Wait for LCD power up
	LCD_sendCommand(0x38); // 8-bit, 2 line, normal font
	LCD_sendCommand(0x39); // Function set
	LCD_sendCommand(0x14); // Internal OSC freq
	LCD_sendCommand(0x70); // Contrast set
	LCD_sendCommand(0x56); // Power/icon/contrast control
	LCD_sendCommand(0x6C); // Follower control
	_delay_ms(200);
	LCD_sendCommand(0x38); // Function set
	LCD_sendCommand(0x0C); // Display ON
	LCD_sendCommand(0x01); // Clear display
	_delay_ms(2);
}
void LCD_setCursor(uint8_t col, uint8_t row) {
	uint8_t address = (row == 0) ? col : (0x40 + col);
	LCD_sendCommand(0x80 | address);
}

void LCD_print(const char *str) {
	while (*str) {
		LCD_sendData(*str++);
	}
}

int main(void) {
	TWI_init();
	LCD_init();

	LCD_setCursor(0, 0);
	LCD_print("Hello, World!");

	LCD_setCursor(0, 1);
	LCD_print("ATmega328PB :)");

	while (1) {
		// Main loop can do other tasks
	}
}