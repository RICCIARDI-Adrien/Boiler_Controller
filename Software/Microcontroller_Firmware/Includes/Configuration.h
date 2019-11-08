/** @file Configuration.h
 * Gather all program configurable parameters.
 * @author Adrien RICCIARDI
 */
#ifndef H_CONFIGURATION_H
#define H_CONFIGURATION_H

//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------
/** The access point to connect to SSID. */
#define CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_SSID "test"
/** The access point to connect to password. */
#define CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_PASSWORD "password"

/** The server to connect to IP address. */
#define CONFIGURATION_PROTOCOL_WIFI_SERVER_ADDRESS "192.168.1.100"
/** The server to connect to port. */
#define CONFIGURATION_PROTOCOL_WIFI_SERVER_PORT "1234"

/** The current firmware version. */
#define CONFIGURATION_FIRMWARE_VERSION 1

/** Mixing valve time in seconds to go from one side to the other side. */
#define CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME (20 * 60) // Valve needs about 18 minutes to travel from one side to the other, set 20 minutes to get some margin (valve has internal limit switches)

/** The reference temperature (in °C) the trimmers use when they are set to 0. */
#define CONFIGURATION_TRIMMERS_REFERENCE_TEMPERATURE 20

/** Minimum temperature value (clamped after heating curve computation). */
#define CONFIGURATION_HEATING_CURVE_MINIMUM_TEMPERATURE 10 // TODO determine a good value
/** Maximum temperature value (clamped after heating curve computation). */
#define CONFIGURATION_HEATING_CURVE_MAXIMUM_TEMPERATURE 70 // TODO determine a good value

/** Add this amount of degrees to the gas burner temperature to reach to avoid turning the gas burner off too often. */
#define CONFIGURATION_GAS_BURNER_TEMPERATURE_HYSTERESIS_HIGH 5
/** Subtract this amount of degrees to the gas burner temperature to reach to avoid turning the gas burner on too often. */
#define CONFIGURATION_GAS_BURNER_TEMPERATURE_HYSTERESIS_LOW 8

/** How many ADC samples to use to compute the moving average value. */
#define CONFIGURATION_ADC_MOVING_AVERAGE_SAMPLES_COUNT 5

/** Heating curve coefficient least significant byte address in internal EEPROM. */
#define CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_LOW_BYTE 0
/** Heating curve coefficient most significant byte address in internal EEPROM. */
#define CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_COEFFICIENT_HIGH_BYTE 1
/** Heating curve parallel shift least significant byte address in internal EEPROM. */
#define CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_LOW_BYTE 2
/** Heating curve parallel shift most significant byte address in internal EEPROM. */
#define CONFIGURATION_EEPROM_ADDRESS_HEATING_CURVE_PARALLEL_SHIFT_HIGH_BYTE 3

#endif
