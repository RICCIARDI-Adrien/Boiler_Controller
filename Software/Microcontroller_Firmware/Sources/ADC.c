/** @file ADC.c
 * @see ADC.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/io.h>
#include <Configuration.h>
#include <Protocol.h>

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** Store all needed values to compute a moving average on ADC sampled values. */
typedef struct
{
	unsigned short Samples[CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT]; //!< Hold all samples used to compute the average.
	unsigned char Oldest_Sample_Index; //!< Tell which is the oldest sample value that can be replaced by a fresh one.
} TADCMovingAverage;

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
	unsigned char i;
	
	// Configure pins as analog
	DDRC &= 0xF0; // Set pins as inputs
	PORTC &= 0xF0; // Put pins in high impedance mode, so digital push-pull stage will not perturb the analog signal even when the pin is not selected as the multiplexer analog input
	DIDR0 = 0x0F; // Disable digital input buffer for the used analog channels
	
	// Configure the ADC module
	ADMUX = 0; // Select AREF pin as voltage reference, right-adjust conversion result
	ADCSRA = 0x85; // Enable ADC module, disable auto triggering feature, set the prescaler to 32 to get a 3686400/32 = 115200Hz ADC clock (ADC clock must be in range 50KHz to 200KHz)
	
	// Sample pins once to discard the first result, which may be wrong, then get as many samples as needed to compute the moving average value in order to have a valid value on program start
	for (i = 0; i < CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT + 1; i++) ADCTask(); // The task will sample several pins (only one sampling is needed to discard the initial bad result), so there is no doubt that ADC values will be sampled fine after that
}

void ADCTask(void)
{
	unsigned char i, j;
	unsigned short Sampled_Value;
	static TADCMovingAverage Moving_Averages[ADC_CHANNEL_IDS_COUNT] = {0};
	TADCMovingAverage *Pointer_Moving_Average;
	unsigned long Averaged_Sampled_Value;
	
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
		
		// Cache moving average access
		Pointer_Moving_Average = &Moving_Averages[i];
		
		// Replace the oldest sample with the new one
		Pointer_Moving_Average->Samples[Pointer_Moving_Average->Oldest_Sample_Index] = Sampled_Value;
		Pointer_Moving_Average->Oldest_Sample_Index++;
		if (Pointer_Moving_Average->Oldest_Sample_Index >= CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT) Pointer_Moving_Average->Oldest_Sample_Index = 0;
		
		// Compute the average
		Averaged_Sampled_Value = 0;
		for (j = 0; j < CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT; j++) Averaged_Sampled_Value += Pointer_Moving_Average->Samples[j];
		Sampled_Value = (unsigned short) (Averaged_Sampled_Value / CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT);
		
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
