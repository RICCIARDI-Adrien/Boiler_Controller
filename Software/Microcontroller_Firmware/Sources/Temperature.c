/** @file Temperature.c
 * @see Temperature.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <Configuration.h>
#include <EEPROM.h>
#include <Protocol.h>
#include <Temperature.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The current desired temperature (in °C), this value is used only when night mode is disabled. */
static signed char Temperature_Desired_Day_Room_Temperature = 0;
/** The current desired temperature (in °C), this value is used only when night mode is enabled. */
static signed char Temperature_Desired_Night_Room_Temperature = 0;

/** The start water temperature to reach (in °C). */
static signed char Temperature_Target_Start_Water_Temperature = 0;

/** Heating curve coefficient (multiplied by ten to make more precise computations). */
static unsigned short Temperature_Heating_Curve_Coefficient;
/** Heating curve parallel shift (also multiplied by ten to improve results precision). */
static unsigned short Temperature_Heating_Curve_Parallel_Shift;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Get day trimmer selected temperature.
 * @return The absolute temperature (in °C) indicated by the day trimmer.
 */
static inline signed char TemperatureGetDayTrimmerTemperature(void)
{
	signed long Temperature;
	
	Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_DAY_TRIMMER);
	
	// Use a straight line representation to determine the Celsius temperature
	// Datasheet tells that temperature is -4°C when trimmer resistance is 60ohm => measured voltage is 900mV => ADC value is 279
	// We need a second point to determine the line equation : temperature is +4 when trimmer resistance is 100ohm => measured voltage is 1.269V => ADC value is 393
	// Straight line equation is Celsius_Temperature = 0.070 * ADC_Value - 23.579, use x1000 fixed arithmetic to keep some precision
	Temperature = ((70L * Temperature) - 23579L) / 1000;
	return (signed char) (Temperature + CONFIGURATION_TRIMMERS_REFERENCE_TEMPERATURE);
}

/** Get night trimmer selected temperature.
 * @return The absolute temperature (in °C) indicated by the night trimmer.
 */
static inline signed char TemperatureGetNightTrimmerTemperature(void)
{
	signed long Temperature;
	
	Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_NIGHT_TRIMMER);
	
	// Use a straight line representation to determine the Celsius temperature
	// Datasheet tells that temperature is 0°C when trimmer resistance is 5ohm => measured voltage is 100mV => ADC value is 31
	// We need a second point to determine the line equation : temperature is +8 when trimmer resistance is 50ohm => measured voltage is 786mV => ADC value is 244
	// Straight line equation is Celsius_Temperature = 0.038 * ADC_Value - 1.164, use x1000 fixed arithmetic to keep some precision
	Temperature = ((38L * Temperature) - 1164L) / 1000;
	return (signed char) (TemperatureGetDayTrimmerTemperature() - Temperature);
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void TemperatureInitialize(void)
{
	// Load heating curve coefficient
	Temperature_Heating_Curve_Coefficient = EEPROMReadByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_HIGH_BYTE) << 8;
	Temperature_Heating_Curve_Coefficient |= EEPROMReadByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_LOW_BYTE);
	
	// Load heating curve parallel shift
	Temperature_Heating_Curve_Parallel_Shift = EEPROMReadByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_HIGH_BYTE) << 8;
	Temperature_Heating_Curve_Parallel_Shift |= EEPROMReadByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_LOW_BYTE);
}

signed char TemperatureGetSensorValue(TTemperatureSensorID Temperature_ID)
{
	signed long Temperature;
	
	switch (Temperature_ID)
	{
		case TEMPERATURE_SENSOR_ID_OUTSIDE:
			Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_OUTSIDE_THERMISTOR);
			// Use a straight line representation to determine the Celsius temperature
			// Datasheet tells that temperature is -10°C when thermistor resistance is 480ohm => measured voltage is 1.667V => ADC value is 516
			// We need a second point to determine the line equation : temperature is 20 when thermistor resistance is 400ohm => measured voltage is 1.517 => ADC value is 470
			// Straight line equation is Celsius_Temperature = -0.652 * ADC_Value + 326.440, use x1000 fixed arithmetic to keep some precision
			Temperature = ((-652L * Temperature) + 326440L) / 1000;
			break;
			
		case TEMPERATURE_SENSOR_ID_RADIATOR_START:
			Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_RADIATOR_START_THERMISTOR);
			// Use a straight line representation to determine the Celsius temperature
			// Datasheet tells that temperature is 20°C when thermistor resistance is 770ohm => measured voltage is 1.436V => ADC value is 445
			// We need a second point to determine the line equation : temperature is 80 when thermistor resistance is 580ohm => measured voltage is 1.211V => ADC value is 375
			// Straight line equation is Celsius_Temperature = -0.857 * ADC_Value + 401.375, use x1000 fixed arithmetic to keep some precision
			Temperature = ((-857L * Temperature) + 401375L) / 1000;
			break;
			
		case TEMPERATURE_SENSOR_ID_RADIATOR_RETURN:
			// TODO when sensor will be chosen
			Temperature = -100;
			break;
			
		default:
			return -100;
	}
	
	return (signed char) Temperature;
}

void TemperatureGetDesiredRoomTemperatures(signed char *Pointer_Day_Temperature, signed char *Pointer_Night_Temperature)
{
	static signed char Previous_Day_Trimmer_Temperature = 0, Previous_Night_Trimmer_Temperature = 0;
	signed char Current_Trimmer_Temperature;
	
	// Change desired day temperature if the day trimmer has been changed
	Current_Trimmer_Temperature = TemperatureGetDayTrimmerTemperature();
	if (Current_Trimmer_Temperature != Previous_Day_Trimmer_Temperature)
	{
		Temperature_Desired_Day_Room_Temperature = Current_Trimmer_Temperature;
		Previous_Day_Trimmer_Temperature = Current_Trimmer_Temperature; // Keep new trimmer temperature to be able to determine next trimmer value change
	}
	
	// Change desired night temperature if the night trimmer has been changed
	Current_Trimmer_Temperature = TemperatureGetNightTrimmerTemperature();
	if (Current_Trimmer_Temperature != Previous_Night_Trimmer_Temperature)
	{
		Temperature_Desired_Night_Room_Temperature = Current_Trimmer_Temperature;
		Previous_Night_Trimmer_Temperature = Current_Trimmer_Temperature; // Keep new trimmer temperature to be able to determine next trimmer value change
	}
	
	*Pointer_Day_Temperature = Temperature_Desired_Day_Room_Temperature;
	*Pointer_Night_Temperature = Temperature_Desired_Night_Room_Temperature;
}

void TemperatureSetDesiredRoomTemperatures(signed char Day_Temperature, signed char Night_Temperature)
{
	Temperature_Desired_Day_Room_Temperature = Day_Temperature;
	Temperature_Desired_Night_Room_Temperature = Night_Temperature;
}

signed char TemperatureGetTargetStartWaterTemperature(void)
{
	return Temperature_Target_Start_Water_Temperature;
}

// Values can only be set by a protocol command, and this function is used exclusively by another protocol command, so there is no need to use a mutex
void TemperatureGetHeatingCurveParameters(unsigned short *Pointer_Coefficient, unsigned short *Pointer_Parallel_Shift)
{
	*Pointer_Coefficient = Temperature_Heating_Curve_Coefficient;
	*Pointer_Parallel_Shift = Temperature_Heating_Curve_Parallel_Shift;
}

// No need for mutex because this function is called exclusively by a protocol command (so this function call interrupts everything else), and all accesses to heating curve variables from main task are secured
void TemperatureSetHeatingCurveParameters(unsigned short Coefficient, unsigned short Parallel_Shift)
{
	// Store coefficient to EEPROM
	EEPROMWriteByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_HIGH_BYTE, Coefficient >> 8);
	EEPROMWriteByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_LOW_BYTE, (unsigned char) Coefficient);
	
	// Store parallel shift to EEPROM
	EEPROMWriteByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_HIGH_BYTE, Parallel_Shift >> 8);
	EEPROMWriteByte(CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_LOW_BYTE, (unsigned char) Parallel_Shift);
	
	// Update variables
	Temperature_Heating_Curve_Coefficient = Coefficient;
	Temperature_Heating_Curve_Parallel_Shift = Parallel_Shift;
}

void TemperatureTask(void)
{
	signed char Outside_Temperature, Desired_Room_Temperature, Target_Start_Water_Temperature, Day_Temperature, Night_Temperature;
	signed long Heating_Curve_Coefficient, Heating_Curve_Parallel_Shift; // Promote unsigned short values to long to force the heating curve computation to be done on long variables
	
	// Use some more variables to make the heating curve computation easier to understand
	Outside_Temperature = TemperatureGetSensorValue(TEMPERATURE_SENSOR_ID_OUTSIDE);
	// Determine the desired room temperature according to current mode
	TemperatureGetDesiredRoomTemperatures(&Day_Temperature, &Night_Temperature);
	if (ProtocolIsNightModeEnabled()) Desired_Room_Temperature = Night_Temperature;
	else Desired_Room_Temperature = Day_Temperature;
	
	// Atomically retrieve values that can be set by Protocol module
	PROTOCOL_DISABLE_INTERRUPTS();
	Heating_Curve_Coefficient = Temperature_Heating_Curve_Coefficient;
	Heating_Curve_Parallel_Shift = Temperature_Heating_Curve_Parallel_Shift;
	PROTOCOL_ENABLE_INTERRUPTS();
	
	// Compute target start water temperature
	Target_Start_Water_Temperature = (Heating_Curve_Coefficient * (Desired_Room_Temperature - Outside_Temperature) + Heating_Curve_Parallel_Shift) / 10L;
	
	// Make sure output value is in the allowed water temperature range
	if (Target_Start_Water_Temperature < CONFIGURATION_HEATING_CURVE_MINIMUM_TEMPERATURE) Target_Start_Water_Temperature = CONFIGURATION_HEATING_CURVE_MINIMUM_TEMPERATURE;
	else if (Target_Start_Water_Temperature > CONFIGURATION_HEATING_CURVE_MAXIMUM_TEMPERATURE) Target_Start_Water_Temperature = CONFIGURATION_HEATING_CURVE_MAXIMUM_TEMPERATURE;
	
	// Update the shared variable now that its value is fully computed
	Temperature_Target_Start_Water_Temperature = Target_Start_Water_Temperature;
}
