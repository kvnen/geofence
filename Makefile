# === Config ===
MCU = atmega328pb
F_CPU = 16000000UL
BAUD = 9600

# === Files ===
SRC = main.c uart.c utils.c nmea.c LCD.c
OBJ = $(SRC:.c=.o)
TARGET = geofence

# === Tools ===
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = -mmcu=$(MCU)

# === Rules ===
all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.o *.elf *.hex

flash: $(TARGET).hex
	avrdude -p atmega328pb -Pusb -c xplainedmini -e -U flash:w:$(TARGET).hex:i

.PHONY: all clean flash

