CC = avr-gcc
# Do not enable compiler optimizations to avoid harmful instructions reordering
CCFLAGS = -W -Wall -mmcu=atmega328p -DF_CPU=3686400UL

PATH_INCLUDES = Includes
PATH_SOURCES = Sources

BINARY = Boiler_Controller_Firmware.elf
INCLUDES = -I$(PATH_INCLUDES)
SOURCES = $(PATH_SOURCES)/ADC.c $(PATH_SOURCES)/Main.c $(PATH_SOURCES)/Protocol.c $(PATH_SOURCES)/Relay.c

PROGRAMMER_SERIAL_PORT ?= /dev/ttyACM0

all:
	$(CC) $(CCFLAGS) $(INCLUDES) $(SOURCES) -o $(BINARY)
	avr-size -C --mcu=atmega328p $(BINARY)

clean:
	rm -f $(BINARY)

flash:
	avrdude -p m328p -c avrisp -b 19200 -P $(PROGRAMMER_SERIAL_PORT) -v -e -U flash:w:$(BINARY) -U lfuse:w:$(BINARY) -U hfuse:w:$(BINARY) -U efuse:w:$(BINARY)
