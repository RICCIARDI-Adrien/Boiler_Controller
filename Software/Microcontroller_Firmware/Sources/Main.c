/** @file Main.c
 * Boiler controller entry point and main loop.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Led.h>
#include <Protocol.h>
#include <Relay.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Configure microcontroller fuses. */
FUSES =
{
	FUSE_CKSEL3, // Fuses low byte : use the longest power-on delay to make sure power supply voltage is stabilized when core starts, select a full swing crystal oscillator with brown-out detection enabled
	FUSE_SPIEN & FUSE_EESAVE, // Fuses high byte : enable Serial programming and Data Downloading, keep EEPROM content when erasing the chip, give less room possible to the bootloader (for now)
	FUSE_BODLEVEL2 // Fuses extended byte : set brown-out reset voltage to approximately 4.3V
};

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(void) // Can't use void return type because it triggers a warning
{
	unsigned char Is_WiFi_Successfully_Initialized, a = 1;
	
	// Initialize modules
	LedInitialize();
	ADCInitialize();
	RelayInitialize();
	Is_WiFi_Successfully_Initialized = ProtocolInitialize();
	
	// Enable interrupts now that all modules have been configured
	sei();
	
	// Tell whether network is working
	if (!Is_WiFi_Successfully_Initialized) LedTurnOn(LED_ID_NETWORK_ERROR);
	
	while (1)
	{
		ADCTask(); // TODO execute this every second but without blocking all other tasks
		
		// TEST
		_delay_ms(1000);
		
		// TEST
		if (a)
		{
			LedTurnOn(LED_ID_STATUS);
			a = 0;
		}
		else
		{
			LedTurnOff(LED_ID_STATUS);
			a = 1;
		}
	}
}
