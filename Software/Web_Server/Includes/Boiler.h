/** @file Boiler.h
 * A TCP server allowing to communicate with the boiler board.
 * @author Adrien RICCIARDI
 */
#ifndef H_BOILER_H
#define H_BOILER_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All available valve positions. */
typedef enum
{
	BOILER_MIXING_VALVE_POSITION_LEFT,
	BOILER_MIXING_VALVE_POSITION_CENTER,
	BOILER_MIXING_VALVE_POSITION_RIGHT
} TBoilerMixingValvePosition;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Start server on default port.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerInitializeServer(void);

/** Gracefully release all resources. */
void BoilerUninitializeServer(void);

/** Wait for the board to connect to the server.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerRunServer(void);

/** Read temperature sensors values.
 * @param Pointer_Outside_Temperature On output, contain the outside temperature in Celsius degrees.
 * @param Pointer_Radiator_Start_Water_Temperature On output, contain the start water temperature in Celsius degrees.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerGetSensorsCelsiusTemperatures(int *Pointer_Outside_Temperature, int *Pointer_Radiator_Start_Water_Temperature);

/** TODO */
int BoilerGetMixingValvePosition(TBoilerMixingValvePosition *Pointer_Position);

/** TODO */
int BoilerSetNightMode(int Is_Night_Mode_Enabled);

/** Read the desired room temperatures.
 * @param Pointer_Day_Temperature On output, contain the desired temperature during the day.
 * @param Pointer_Night_Temperature On output, contain the desired temperature during the night.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerGetDesiredRoomTemperatures(int *Pointer_Day_Temperature, int *Pointer_Night_Temperature);

/** TODO */
int BoilerSetDesiredRoomTemperature(int Temperature);

/** Tell whether boiler is running or is idle.
 * @param Pointer_Is_Boiler_Running On output, is equal to 1 if the boiler is running or is equal to 0 if the boiler is idle.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerGetBoilerRunningMode(int *Pointer_Is_Boiler_Running);

/** Put boiler in running or idle mode.
 * @param Is_Boiler_Running Set to 1 to put boiler in running mode, set to 0 to put boiler in idle mode.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
int BoilerSetBoilerRunningMode(int Is_Boiler_Running);

/** TODO */
int BoilerGetTargetRadiatorStartWaterTemperature(int *Pointer_Temperature);

#endif
