/** @file Main.c
 * Boiler controller entry point and main loop.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/io.h>
#include <avr/interrupt.h>
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
	// Initialize modules
	ADCInitialize();
	ProtocolInitialize();
	RelayInitialize();
	
	// Enable interrupts now that all modules have been configured
	sei();
	
	// TEST
	DDRD |= 0x04;
	/*while (1)
	{
		PORTD |= 0x04;
		_delay_ms(1000);
		
		PORTD &= ~0x04;
		_delay_ms(1000);
	}*/
	while (1)
	{
		ADCTask(); // TODO execute this every second but without blocking all other tasks
		
		// TEST
		_delay_ms(1000);
		
		// TEST
		if (PIND & 0x04) PORTD &= ~0x04;
		else PORTD |= 0x04;
		
		RelayTurnOn(RELAY_ID_MIXING_VALVE_HOTTER);
		_delay_ms(100);
		RelayTurnOn(RELAY_ID_MIXING_VALVE_COLDER);
		_delay_ms(100);
		RelayTurnOn(RELAY_ID_GAS_BURNER);
		_delay_ms(100);
		RelayTurnOn(RELAY_ID_PUMP);
		_delay_ms(300);
		
		RelayTurnOff(RELAY_ID_MIXING_VALVE_HOTTER);
		RelayTurnOff(RELAY_ID_MIXING_VALVE_COLDER);
		RelayTurnOff(RELAY_ID_GAS_BURNER);
		RelayTurnOff(RELAY_ID_PUMP);
	}
}
