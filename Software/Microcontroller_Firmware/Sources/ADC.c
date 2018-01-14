/** @file ADC.c
 * @see ADC.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/io.h>
#include <Protocol.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Hold all sampled values. */
static unsigned short ADC_Sampled_Values[ADC_CHANNEL_IDS_COUNT];

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void ADCInitialize(void)
{
	// Configure pins as analog
	DDRC &= 0xE0; // Set pins as inputs
	PORTC &= 0xE0; // Put pins in high impedance mode, so digital push-pull stage will not perturb the analog signal even when the pin is not selected as the multiplexer analog input
	DIDR0 = 0x1F; // Disable digital input buffer for the used analog channels
	
	// Configure the ADC module
	ADMUX = 0; // Select AREF pin as voltage reference, right-adjust conversion result
	ADCSRA = 0x85; // Enable ADC module, disable auto triggering feature, set the prescaler to 32 to get a 3686400/32 = 115200Hz ADC clock (ADC clock must be in range 50KHz to 200KHz)
	
	// Sample pins once to discard the first result, which may be wrong
	ADCTask(); // The task will sample several pins (only one sampling is needed to discard the initial bad result), so there is no doubt that ADC values will be sampled fine after that
}

void ADCTask(void)
{
	unsigned char i;
	unsigned short Sampled_Value;
	
	for (i = 0; i < ADC_CHANNEL_IDS_COUNT; i++)
	{
		// Select the channel to sample
		ADMUX &= 0xF0; // Reset the channel mask to 0
		ADMUX |= i;
		
		// Start operation
		ADCSRA |= 1 << 6;
		
		// Wait for the conversion to finish
		while (!(ADCSRA & (1 << 4)));
		ADCSRA &= ~(1 << 4); // Clear interrupt flag
		
		// Get the sampled value
		Sampled_Value = ADCL; // Reading low register first results in latching the high register, even if it is useless here due to no other conversion started
		Sampled_Value |= ADCH << 8;
		
		// Atomically update the sampled value (Protocol module may access to this value at any time, so disable Protocol interrupts to avoid it reading a semi-updated value)
		PROTOCOL_DISABLE_INTERRUPTS();
		ADC_Sampled_Values[i] = Sampled_Value;
		PROTOCOL_ENABLE_INTERRUPTS();
	}
}

unsigned short ADCGetLastSampledValue(TADCChannelID Channel_ID)
{
	// Make sure the provided channel is existing
	if (Channel_ID >= ADC_CHANNEL_IDS_COUNT) return 0;
	
	// No need for a "mutex" here because the function is called by Protocol module (this call is protected by a mutex yet) or by main(), which also calls the ADC sampling task, so everything is synchronized
	return ADC_Sampled_Values[Channel_ID];
}
