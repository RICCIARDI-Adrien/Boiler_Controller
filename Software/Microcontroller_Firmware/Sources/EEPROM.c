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

void EEPROMWriteByte(unsigned short Address, unsigned char Data)
{
	// Configure write address
	Address &= 0x03FF; // Make sure address is valid, ATMEGA328P has 1KB of EEPROM space
	EEARH = Address >> 8;
	EEARL = (unsigned char) Address;

	// Configure data to write
	EEDR = Data;

	// Write byte
	EECR = 0x04; // Initialize write cycle by setting master write enable bit, select write and erase in a single operation
	EECR |= 0x02; // Start writing

	// Wait for write cycle to terminate
	while (EECR & 0x02); // Wait for EEPE bit to be cleared by hardware
}
