/** @file Relay.h
 * Simple wrapper to easily control the board relays.
 * @author Adrien RICCIARDI
 */
#ifndef H_RELAY_H
#define H_RELAY_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All available relays. */
typedef enum
{
	RELAY_ID_MIXING_VALVE_LEFT = 4, //!< Connected to PD4 pin.
	RELAY_ID_MIXING_VALVE_RIGHT = 5, //!< Connected to PD5 pin.
	RELAY_ID_GAS_BURNER = 6, //!< Connected to PD6 pin.
	RELAY_ID_PUMP = 7 //!< Connected to PD7 pin.
} TRelayID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Configure the needed GPIOs to access relays. */
void RelayInitialize(void);

/** Close a relay circuit.
 * @param Relay_ID The relay to close.
 */
void RelayTurnOn(TRelayID Relay_ID);

/** Open a relay circuit.
 * @param Relay_ID The relay to open.
 */
void RelayTurnOff(TRelayID Relay_ID);

#endif
