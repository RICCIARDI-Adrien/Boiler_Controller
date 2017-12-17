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
/** Put valve in a known position (controller board has no way to know where the valve is).
 * @note This function takes a fairly amount of time because valve is moving very slowly.
 */
void MixingValveInitialize(void);

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

/** This task must be called each second, it controls the relays that makes the mixing valve move. */
void MixingValveTask(void);

#endif
