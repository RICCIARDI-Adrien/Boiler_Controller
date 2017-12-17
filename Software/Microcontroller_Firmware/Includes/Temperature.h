/** @file Temperature.h
 * Allow to easily convert the temperatures read from all different sensors to Celsius degrees.
 * @author Adrien RICCIARDI
 */
#ifndef H_TEMPERATURE_H
#define H_TEMPERATURE_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All existing temperature sensors. */
typedef enum
{
	TEMPERATURE_ID_OUTSIDE, //<! External sensor temperature.
	TEMPERATURE_ID_RADIATOR_START, //<! The pipe going to the radiators' temperature.
	TEMPERATURE_ID_RADIATOR_RETURN, //<! The pipe coming from the radiators' temperature.
	TEMPERATURE_IDS_COUNT
} TTemperatureID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Convert a specific temperature to Celsius degrees.
 * @param Temperature_ID The temperature to get °C value.
 * @return The temperature converted to °C.
 * @note Function will return -100 if the provided temperature ID is bad (to notify that something is wrong).
 */
signed char TemperatureGetCelsiusValue(TTemperatureID Temperature_ID);

#endif
