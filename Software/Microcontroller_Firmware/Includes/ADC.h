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
	ADC_CHANNEL_ID_EXTERNAL_THERMISTOR,
	ADC_CHANNEL_ID_DAY_TRIMMER,
	ADC_CHANNEL_ID_NIGHT_TRIMMER,
	ADC_CHANNEL_ID_INTERNAL_THERMISTOR,
	ADC_CHANNEL_IDS_COUNT
} TADCChannelID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Initialize desired analog pins and ADC module. */
void ADCInitialize(void);

/** Sample all channels. Must be called periodically. */
void ADCTask(void);

#endif