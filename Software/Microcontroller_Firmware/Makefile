CC = avr-gcc
CCFLAGS = -W -Wall -mmcu=atmega328p -DF_CPU=3686400UL

PATH_SOURCES = Sources

BINARY = Boiler_Controller_Firmware.elf
SOURCES = $(PATH_SOURCES)/Main.c

PROGRAMMER_SERIAL_PORT = /dev/ttyACM0

all:
	$(CC) $(CCFLAGS) $(SOURCES) -o $(BINARY)

clean:
	rm -f $(BINARY)

flash:
	avrdude -p m328p -c avrisp -b 19200 -P $(PROGRAMMER_SERIAL_PORT) -v -e -U flash:w:$(BINARY)
