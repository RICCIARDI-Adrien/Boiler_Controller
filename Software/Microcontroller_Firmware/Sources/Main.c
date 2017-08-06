/** @file Main.c
 * Boiler controller entry point.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <Protocol.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Configure microcontroller fuses. */
FUSES =
{
	FUSE_SUT1 & FUSE_SUT0 & FUSE_CKSEL2 & FUSE_CKSEL1 & FUSE_CKSEL0, // Fuses low byte : use the longest power-on delay to make sure power supply voltage is stabilized when core starts, select a full swing crystal oscillator with brown-out detection enabled
	FUSE_RSTDISBL & FUSE_DWEN & FUSE_EESAVE & FUSE_BOOTSZ1 & FUSE_BOOTSZ0, // Fuses high byte : disable external reset, enable Serial Program and Data Downloading, keep EEPROM content when erasing the chip, give less room possible to the bootloader (for now)
	FUSE_BODLEVEL2 // Fuses extended byte : set brown-out reset voltage to approximately 4.3V
};

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(void) // Can't use void return type because it triggers a warning
{
	// Initialize modules
	ProtocolInitialize();
	
	// TEST
	DDRD = 0x04;
	while (1)
	{
		PORTD |= 0x04;
		_delay_ms(1000);
		
		PORTD &= ~0x04;
		_delay_ms(1000);
	}
}
