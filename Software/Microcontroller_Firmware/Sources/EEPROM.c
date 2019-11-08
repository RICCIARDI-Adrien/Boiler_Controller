/** @file EEPROM.c
 * See EEPROM.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/eeprom.h>
#include <EEPROM.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
unsigned char EEPROMReadByte(unsigned short Address)
{
	// Configure read address
	Address &= 0x03FF; // Make sure address is valid, ATMEGA328P has 1KB of EEPROM space
	EEARH = Address >> 8;
	EEARL = (unsigned char) Address;

	// Read byte
	EECR = 0x01;
	return EEDR;
}
