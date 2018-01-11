/** @file Led.c
 * @see Led.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <Led.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void LedInitialize(void)
{
	// Turn all leds off
	PORTB &= ~0x07;
	PORTD &= ~0x0C;
	
	// Set led ports as outputs
	DDRB |= 0x03;
	DDRD |= 0x0C;
}

void LedTurnOn(TLedID Led_ID)
{
	switch (Led_ID)
	{
		case LED_ID_STATUS:
			PORTD |= 0x04;
			break;
		
		case LED_ID_NETWORK_ERROR:
			PORTD |= 0x08;
			break;
			
		case LED_ID_LED_3:
			PORTB |= 0x01;
			break;
			
		case LED_ID_LED_4:
			PORTB |= 0x02;
			break;
			
		default:
			break;
	}
}

void LedTurnOff(TLedID Led_ID)
{
	switch (Led_ID)
	{
		case LED_ID_STATUS:
			PORTD &= ~0x04;
			break;
		
		case LED_ID_NETWORK_ERROR:
			PORTD &= ~0x08;
			break;
			
		case LED_ID_LED_3:
			PORTB &= ~0x01;
			break;
			
		case LED_ID_LED_4:
			PORTB &= ~0x02;
			break;
			
		default:
			break;
	}
}
