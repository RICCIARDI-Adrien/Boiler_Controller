/** @file Mixing_Valve.c
 * @see Mixing_Valve.h for description.
 * @author Adrien RICCIARDI
 */
#include <Configuration.h>
#include <Mixing_Valve.h>
#include <Protocol.h>
#include <Relay.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The current position. */
static TMixingValvePosition Mixing_Valve_Current_Position = MIXING_VALVE_POSITION_LEFT; // Assume valve is located left to use the maximum moving time to put it to right position
/** The position the valve must reach. */
static TMixingValvePosition Mixing_Valve_Target_Position;

/** How many seconds the valve needs to travel from one side to the other. */
static unsigned short Mixing_Valve_Maximum_Moving_Time = CONFIGURATION_MIXING_VALVE_MAXIMUM_MOVING_TIME;
/** How many seconds the mixing valve task must wait before stopping the relays. */
static unsigned short Mixing_Valve_Remaining_Moving_Time = 0;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void MixingValveInitialize(void)
{
	// Make sure the valve is in the right position
	MixingValveSetPosition(MIXING_VALVE_POSITION_RIGHT);
	
	while (Mixing_Valve_Current_Position != MIXING_VALVE_POSITION_RIGHT)
	{
		MixingValveTask();
		_delay_ms(1000);
	}
}

// No need for mutex, value is one byte wide only (and even if this is the previous value just before an update that is read it's not a problem)
TMixingValvePosition MixingValveGetPosition(void)
{
	return Mixing_Valve_Current_Position;
}

// No need for mutex because this function is called sequentially with mixing valve task function
void MixingValveSetPosition(TMixingValvePosition Position)
{
	unsigned short Maximum_Moving_Time;
	
	// Nothing to do if the valve is in the desired position yet
	if (Position == Mixing_Valve_Current_Position) return;
	
	// Access moving time value through a "mutex" because this value can be changed at any time by the protocol module
	PROTOCOL_DISABLE_INTERRUPTS();
	Maximum_Moving_Time = Mixing_Valve_Maximum_Moving_Time;
	PROTOCOL_ENABLE_INTERRUPTS();
	
	// Compute the required moving time and activate the needed relays
	switch (Position)
	{
		case MIXING_VALVE_POSITION_LEFT:
			// Make the valve moves
			RelayTurnOn(RELAY_ID_MIXING_VALVE_LEFT);
			Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_LEFT;
			// Compute the needed moving time
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_CENTER) Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time / 2; // Go from center to left
			else Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time; // Go from right to left
			break;
			
		case MIXING_VALVE_POSITION_CENTER:
			// Go from left to center
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_LEFT)
			{
				RelayTurnOn(RELAY_ID_MIXING_VALVE_RIGHT);
				Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_CENTER;
			}
			// Go from right to center
			else
			{
				RelayTurnOn(RELAY_ID_MIXING_VALVE_LEFT);
				Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_CENTER;
			}
			Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time / 2;
			break;
			
		case MIXING_VALVE_POSITION_RIGHT:
			// Make the valve moves
			RelayTurnOn(RELAY_ID_MIXING_VALVE_RIGHT);
			Mixing_Valve_Target_Position = MIXING_VALVE_POSITION_RIGHT;
			// Compute the needed moving time
			if (Mixing_Valve_Current_Position == MIXING_VALVE_POSITION_CENTER) Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time / 2; // Go from center to right
			else Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time; // Go from left to right
			break;
			
		// Should not reach this code
		default:
			break;
	}
}

void MixingValveSetMaximumMovingTime(unsigned short Maximum_Moving_Time)
{
	// It is safe to directly set this variable because Mixing_Valve_Remaining_Moving_Time access is protected in the required functions
	Mixing_Valve_Remaining_Moving_Time = Maximum_Moving_Time;
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
	}
}
