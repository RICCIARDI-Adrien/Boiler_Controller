CC = avr-gcc
# Do not enable compiler optimizations to avoid harmful instructions reordering, relocalize code and data to bootloader space (bootloader address is 0x3800, but this value is an AVR 16-bit word offset which needs to be multiplied by two to become the bytes value expected by the linker), see AVR231 "AES bootloader" application note for some details about bootloader configuration
CCFLAGS = -W -Wall -mmcu=atmega328p -DF_CPU=3686400UL -Wl,--section-start=.text=0x7000 -ffunction-sections -fdata-sections -nostartfiles -nodefaultlibs -nostdlib -Wl,-static

PATH_INCLUDES = Includes
PATH_SOURCES = Sources

BINARY = Boiler_Controller_Bootloader.elf
INCLUDES = -I$(PATH_INCLUDES)
SOURCES = $(PATH_SOURCES)/Main.c $(PATH_SOURCES)/UART.c

PROGRAMMER_SERIAL_PORT ?= /dev/ttyACM0

all:
	$(CC) $(CCFLAGS) $(INCLUDES) $(SOURCES) -o $(BINARY)

clean:
	rm -f $(BINARY)

flash:
	avrdude -p m328p -c avrisp -b 19200 -P $(PROGRAMMER_SERIAL_PORT) -v -e -U flash:w:../Microcontroller_Firmware/Boiler_Controller_Firmware.elf -U flash:w:$(BINARY) -U lfuse:w:$(BINARY) -U hfuse:w:$(BINARY) -U efuse:w:$(BINARY)
	#avrdude -p m328p -c avrisp -b 19200 -P $(PROGRAMMER_SERIAL_PORT) -v -e -U flash:w:$(BINARY) -U lfuse:w:$(BINARY) -U hfuse:w:$(BINARY) -U efuse:w:$(BINARY)
