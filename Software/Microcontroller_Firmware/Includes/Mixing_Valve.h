/** @file Mixing_Valve.h
 * Handle mixing valve moves and allow Protocol module to access the valve state at any time.
 * @author Adrien RICCIARDI
 */
#ifndef H_MIXING_VALVE_H
#define H_MIXING_VALVE_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All available valve positions. */
typedef enum
{
	MIXING_VALVE_POSITION_LEFT,
	MIXING_VALVE_POSITION_CENTER,
	MIXING_VALVE_POSITION_RIGHT
} TMixingValvePosition;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Move the mixing valve to the specified position.
 * @param Position The position the mixing valve must reach.
 * @note Function does nothing if the mixing valve is in the expected position yet.
 */
void MixingValveSetPosition(TMixingValvePosition Position);

/** Tell the current valve position.
 * @return The current valve position.
 * @note Position is updated only when valve has finished moving, so the current position may be inaccurate when the valve is moving.
 */
TMixingValvePosition MixingValveGetPosition(void);

/** Set the time in seconds needed for the valve to travel from one side to the other.
 * @param Maximum_Moving_Time The time value in seconds.
 */
void MixingValveSetMaximumMovingTime(unsigned short Maximum_Moving_Time);

/** This task must be called each second, it controls the relays that makes the mixing valve move. */
void MixingValveTask(void);

#endif
