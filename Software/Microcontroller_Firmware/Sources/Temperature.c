/** @file Temperature.c
 * @see Temperature.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <Temperature.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
signed char TemperatureGetCelsiusValue(TTemperatureID Temperature_ID)
{
	signed long Temperature;
	
	switch (Temperature_ID)
	{
		case TEMPERATURE_ID_OUTSIDE:
			Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_OUTSIDE_THERMISTOR);
			// Use a straight line representation to determine the Celsius temperature
			// Datasheet tells that temperature is -10°C when thermistor resistance is 480ohm => measured voltage is 1.667V => ADC value is 516
			// We need a second point to determine the line equation : temperature is 20 when thermistor resistance is 400ohm => measured voltage is 1.517 => ADC value is 470
			// Straight line equation is Celsius_Temperature = -0.652 * ADC_Value + 326.440, use x1000 fixed arithmetic to keep some precision
			Temperature = ((-652L * Temperature) + 326440L) / 1000;
			break;
			
		case TEMPERATURE_ID_RADIATOR_START:
			Temperature = ADCGetLastSampledValue(ADC_CHANNEL_ID_RADIATOR_START_THERMISTOR);
			// Use a straight line representation to determine the Celsius temperature
			// Datasheet tells that temperature is 20°C when thermistor resistance is 770ohm => measured voltage is 1.436V => ADC value is 445
			// We need a second point to determine the line equation : temperature is 80 when thermistor resistance is 580ohm => measured voltage is 1.211V => ADC value is 375
			// Straight line equation is Celsius_Temperature = -0.857 * ADC_Value + 401.375, use x1000 fixed arithmetic to keep some precision
			Temperature = ((-857L * Temperature) + 401375L) / 1000;
			break;
			
		case TEMPERATURE_ID_RADIATOR_RETURN:
			// TODO when sensor will be chosen
			Temperature = -100;
			break;
			
		default:
			return -100;
	}
	
	return (signed char) Temperature;
}
