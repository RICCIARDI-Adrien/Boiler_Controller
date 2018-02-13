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
	TEMPERATURE_SENSOR_ID_OUTSIDE, //<! External sensor temperature.
	TEMPERATURE_SENSOR_ID_RADIATOR_START, //<! The pipe going to the radiators' temperature.
	TEMPERATURE_SENSOR_ID_RADIATOR_RETURN, //<! The pipe coming from the radiators' temperature.
	TEMPERATURE_SENSOR_IDS_COUNT
} TTemperatureSensorID;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Convert a specific sensor temperature to Celsius degrees.
 * @param Temperature_ID The sensor to get °C temperature value.
 * @return The temperature converted to °C.
 * @note Function will return -100 if the provided temperature ID is bad (to notify that something is wrong).
 */
signed char TemperatureGetSensorValue(TTemperatureSensorID Temperature_ID);

/** Determine the desired room temperature according to trimmers position (taking night and day into account) and protocol command.
 * @return The desired room temperature in °C.
 */
signed char TemperatureGetDesiredRoomTemperature(void);

/** Set the desired room temperature. This value can be overwritten by a trimmer position change.
 * @param Temperature The new desired room temperature.
 */
void TemperatureSetDesiredRoomTemperature(signed char Temperature);

/** Turn on or off night mode (needed do know what trimmer to monitor for temperature change).
 * @param Is_Night_Mode_Enabled Set to 1 to enable night mode, set to 0 to enable day mode.
 */
void TemperatureSetNightMode(unsigned char Is_Night_Mode_Enabled);

/** Get the last computed start water temperature to reach.
 * @return The target start water temperature.
 */
signed char TemperatureGetTargetStartWaterTemperature(void);

/** Retrieve heating curve current settings.
 * @param Pointer_Coefficient On output, contain the coefficient multiplied by ten.
 * @param Pointer_Parallel_Shift On output, contain the parallel shift multiplied by ten.
 */
void TemperatureGetHeatingCurveParameters(unsigned short *Pointer_Coefficient, unsigned short *Pointer_Parallel_Shift);

/** Set heating curve settings. They will be used on next TemperatureTask() call.
 * @param Coefficient The heating curve coefficient, it must be multiplied by ten.
 * @param Parallel_Shift The heating curve parallel shift, it must be multiplied by ten.
 */
void TemperatureSetHeatingCurveParameters(unsigned short Coefficient, unsigned short Parallel_Shift);

/** Compute the target start water temperature using the heating curve. Must be called periodically. */
void TemperatureTask(void);

#endif
