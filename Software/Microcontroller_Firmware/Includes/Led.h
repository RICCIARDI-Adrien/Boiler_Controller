/** @file Led.h
 * Turn on and off a specific led in a simple way.
 * @author Adrien RICCIARDI
 */
#ifndef H_LED_H
#define H_LED_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All available leds. */
typedef enum
{
	LED_ID_STATUS,
	LED_ID_NETWORK_ERROR,
	LED_ID_BOILER_RUNNING_MODE,
	LED_ID_LED_4
} TLedID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Configure all needed ports as outputs. */
void LedInitialize(void);

/** Turn a led on. */
void LedTurnOn(TLedID Led_ID);

/** Turn a led off. */
void LedTurnOff(TLedID Led_ID);

#endif
