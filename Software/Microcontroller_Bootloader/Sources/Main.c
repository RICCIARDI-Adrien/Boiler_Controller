/** @file Main.c
 * Boiler controller bootloader entry point and main loop.
 * @author Adrien RICCIARDI
 */

// TEST
#include <avr/io.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Configure microcontroller fuses. */
FUSES =
{
	FUSE_SUT1 & FUSE_SUT0 & FUSE_CKSEL2 & FUSE_CKSEL1 & FUSE_CKSEL0, // Fuses low byte : use the longest power-on delay to make sure power supply voltage is stabilized when core starts, select a full swing crystal oscillator with brown-out detection enabled
	FUSE_SPIEN & FUSE_EESAVE & FUSE_BOOTRST, // Fuses high byte : enable Serial programming and Data Downloading, keep EEPROM content when erasing the chip, boot from bootloader memory space
	FUSE_BODLEVEL2 // Fuses extended byte : set brown-out reset voltage to approximately 4.3V
};

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(void) // Can't use void return type because it triggers a warning
{
	// TEST
	int i;
	
	// TEST
	DDRD = 0x04;
	for (i = 0; i < 10; i++)
	{
		PORTD |= 0x04;
		
		//_delay_ms(250);
		{
			volatile long a;
			for (a = 0; a < 20000; a++);
		}
		PORTD &= ~0x04;
		
		//_delay_ms(250);
		{
			volatile long a;
			for (a = 0; a < 20000; a++);
		}
	}
	
	// Go to program space
	asm volatile
	(
		"jmp 0"
		:
		:
	);
}
