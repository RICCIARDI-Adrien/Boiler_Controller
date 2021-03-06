/** @file Mixing_Valve.c
 * @see Mixing_Valve.h for description.
 * @author Adrien RICCIARDI
 */
#include <Configuration.h>
#include <Led.h>
#include <Mixing_Valve.h>
#include <Relay.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The current position. */
static TMixingValvePosition Mixing_Valve_Current_Position = MIXING_VALVE_POSITION_LEFT; // Assume valve is located left as it should have been left by "idle mode" code
/** The position the valve must reach. */
static TMixingValvePosition Mixing_Valve_Target_Position;

/** How many seconds the mixing valve task must wait before stopping the relays. */
static unsigned short Mixing_Valve_Remaining_Moving_Time = 0;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
// No need for mutex, value is one byte wide only (and even if this is the previous value just before an update that is read it's not a problem)
TMixingValvePosition MixingValveGetPosition(void)
{
	return Mixing_Valve_Current_Position;
}

// No need for mutex because this function is called sequentially with mixing valve task function
void MixingValveSetPosition(TMixingValvePosition Position)
{
	// Compute the required moving time and activate the needed relays
	switch (Position)
	{
		case MIXING_VALVE_POSITION_LEFT:
			// Make the valve moves
			RelayTurnOn(RELAY_ID_MIXING_VALVE_LEFT);
			RelayTurnOff(RELAY_ID_MIXING_VALVE_RIGHT);
			Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_LEFT;
			// Compute the needed moving time
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_CENTER) Mixing_Valve_Remaining_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME / 2; // Go from center to left
			else Mixing_Valve_Remaining_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME; // Go from right to left
			break;
			
		case MIXING_VALVE_POSITION_CENTER:
			// Go from left to center
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_LEFT)
			{
				RelayTurnOff(RELAY_ID_MIXING_VALVE_LEFT);
				RelayTurnOn(RELAY_ID_MIXING_VALVE_RIGHT);
			}
			// Go from right to center
			else
			{
				RelayTurnOn(RELAY_ID_MIXING_VALVE_LEFT);
				RelayTurnOff(RELAY_ID_MIXING_VALVE_RIGHT);
			}
			Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_CENTER;
			Mixing_Valve_Remaining_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME / 2;
			break;
			
		case MIXING_VALVE_POSITION_RIGHT:
			// Make the valve moves
			RelayTurnOff(RELAY_ID_MIXING_VALVE_LEFT);
			RelayTurnOn(RELAY_ID_MIXING_VALVE_RIGHT);
			Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_RIGHT;
			// Compute the needed moving time
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_CENTER) Mixing_Valve_Remaining_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME / 2; // Go from center to right
			else Mixing_Valve_Remaining_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME; // Go from left to right
			break;
			
		// Should not reach this code
		default:
			break;
	}
	
	// Tell user that mixing valve is moving
	LedTurnOn(LED_ID_MIXING_VALVE_MOVING); // The led will never turn off if an invalid position is specified but this should never happen
}

void MixingValveTask(void)
{
	// Nothing to do if the valve is not moving
	if (Mixing_Valve_Remaining_Moving_Time == 0) return;
	
	// One more second has elapsed
	Mixing_Valve_Remaining_Moving_Time--;
	
	if (Mixing_Valve_Remaining_Moving_Time == 0)
	{
		// Stop moving
		RelayTurnOff(RELAY_ID_MIXING_VALVE_LEFT);
		RelayTurnOff(RELAY_ID_MIXING_VALVE_RIGHT);
		
		// Valve has reached the target position
		Mixing_Valve_Current_Position = Mixing_Valve_Target_Position;
		
		// Tell user that mixing valve finished moving
		LedTurnOff(LED_ID_MIXING_VALVE_MOVING);
	}
}
