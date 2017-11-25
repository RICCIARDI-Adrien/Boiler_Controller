/** @file Relay.c
 * @see Relay.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <Relay.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void RelayInitialize(void)
{
	DDRD |= 0xF0;
}

void RelayTurnOn(TRelayID Relay_ID)
{
	PORTD |= 1 << Relay_ID;
}

void RelayTurnOff(TRelayID Relay_ID)
{
	PORTD &= ~(1 << Relay_ID);
}
