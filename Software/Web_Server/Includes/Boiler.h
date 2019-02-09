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

/** TODO */
int BoilerRunServer(void);

/** TODO */
int BoilerGetSensorsCelciusTemperatures(int *Pointer_Outside_Temperature, int *Pointer_Radiator_Start_Water_Temperature);

/** TODO */
int BoilerGetMixingValvePosition(TBoilerMixingValvePosition *Pointer_Position);

/** TODO */
int BoilerSetNightMode(int Is_Night_Mode_Enabled);

/** TODO */
int BoilerGetDesiredRoomTemperature(int *Pointer_Temperature);

/** TODO */
int BoilerSetDesiredRoomTemperature(int Temperature);

/** TODO */
int BoilerGetBoilerRunningMode(int *Pointer_Is_Boiler_Runnning);

/** TODO */
int BoilerSetBoilerRunningMode(int Is_Boiler_Running);

/** TODO */
int BoilerGetTargetRadiatorStartWaterTemperature(int *Pointer_Temperature);

#endif
