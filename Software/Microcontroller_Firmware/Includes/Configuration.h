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
#define CONFIGURATION_PROTOCOL_WIFI_SERVER_ADDRESS "192.168.1.39"
/** The server to connect to port. */
#define CONFIGURATION_PROTOCOL_WIFI_SERVER_PORT "1234"

/** The current firmware version. */
#define CONFIGURATION_FIRMWARE_VERSION 1

/** Mixing valve time in seconds to go from one side to the other side. */
#define CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME (20 * 60) // Valve needs about 18 minutes to travel from one side to the other, set 20 minutes to get some margin (valves has internal limit switches)

/** The reference temperature (in °C) the trimmers use when they are set to 0. */
#define CONFIGURATION_TRIMMERS_REFERENCE_TEMPERATURE 20

/** The curve coefficient (in °C) x10 to improve computation results. */
#define CONFIGURATION_HEATING_CURVE_COEFFICIENT 14L
/** The curve parallel shift (offset to vertically add to the curve), also x10 to be compatible with the curve coefficient. */
#define CONFIGURATION_HEATING_CURVE_PARALLEL_SHIFT 150L
/** Minimum temperature value (clamped after heating curve computation). */
#define CONFIGURATION_HEATING_CURVE_MINIMUM_TEMPERATURE 10 // TODO determine a good value
/** Maximum temperature value (clamped after heating curve computation). */
#define CONFIGURATION_HEATING_CURVE_MAXIMUM_TEMPERATURE 70 // TODO determine a good value

/** Add or subtract this amount of degrees to the gas burner temperature to reach to avoid turning the gas burner on or off too often. */
#define CONFIGURATION_GAS_BURNER_TEMPERATURE_HYSTERESIS 5

#endif
