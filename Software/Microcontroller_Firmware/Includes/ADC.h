/** @file ADC.h
 * Sample all needed analog channels.
 * @author Adrien RICCIARDI
 */
#ifndef H_ADC_H
#define H_ADC_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All available analog channels. Declare them in the same order they are assigned to analog pins (PC0 first, then PC1, ...). */
typedef enum
{
	ADC_CHANNEL_ID_OUTSIDE_THERMISTOR,
	ADC_CHANNEL_ID_DAY_TRIMMER,
	ADC_CHANNEL_ID_NIGHT_TRIMMER,
	ADC_CHANNEL_ID_RADIATOR_START_THERMISTOR,
	ADC_CHANNEL_ID_RADIATOR_RETURN_THERMISTOR,
	ADC_CHANNEL_IDS_COUNT
} TADCChannelID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Initialize desired analog pins and ADC module. */
void ADCInitialize(void);

/** Sample all channels. Must be called periodically. */
void ADCTask(void);

/** Get a specific channel last sampled value.
 * @param Channel_ID The channel to get value from.
 * @return The last sampled value, in 10-bit raw ADC units,
 * @return 0 if the provided channel does not exist.
 */
unsigned short ADCGetLastSampledValue(TADCChannelID Channel_ID);

#endif
