/** @file EEPROM.h
 * Simple access to device internal EEPROM memory.
 * @author Adrien RICCIARDI
 */
#ifndef H_EEPROM_H
#define H_EEPROM_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Read an EEPROM byte value.
 * @param Address The byte address in range [0..1023].
 * @return The byte value.
 */
unsigned char EEPROMReadByte(unsigned short Address);

#endif
