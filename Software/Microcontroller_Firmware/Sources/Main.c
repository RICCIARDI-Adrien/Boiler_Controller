/** @file Main.c
 * Boiler controller entry point and main loop.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <Configuration.h>
#include <Led.h>
#include <Mixing_Valve.h>
#include <Protocol.h>
#include <Relay.h>
#include <Temperature.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** Configure microcontroller fuses. */
FUSES =
{
	FUSE_CKSEL3, // Fuses low byte : use the longest power-on delay to make sure power supply voltage is stabilized when core starts, select a full swing crystal oscillator with brown-out detection enabled
	FUSE_SPIEN & FUSE_EESAVE, // Fuses high byte : enable Serial programming and Data Downloading, keep EEPROM content when erasing the chip, give less room possible to the bootloader (for now)
	FUSE_BODLEVEL2 // Fuses extended byte : set brown-out reset voltage to approximately 4.3V
};

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(void) // Can't use void return type because it triggers a warning
{
	unsigned char Is_WiFi_Successfully_Initialized, Is_Status_Led_On = 1;
	signed char Radiator_Water_Start_Temperature, Target_Start_Water_Temperature;
	
	// Initialize modules
	LedInitialize();
	LedTurnOn(LED_ID_STATUS); // Turn status led on to tell controller is booting
	ADCInitialize();
	RelayInitialize();
	Is_WiFi_Successfully_Initialized = ProtocolInitialize();
	
	// Enable interrupts now that all modules have been configured
	sei();
	
	// Tell whether network is working
	if (!Is_WiFi_Successfully_Initialized) LedTurnOn(LED_ID_NETWORK_ERROR);
	
	while (1)
	{
		// Sample all analog values
		ADCTask();
		
		// Compute target temperature to reach (compute it even when boiler is not running in order to report a good value through protocol commands)
		TemperatureTask();
		
		if (ProtocolIsBoilerRunning())
		{
			// Cache converted temperature values (conversion computations cost a lot of cycles)
			Radiator_Water_Start_Temperature = TemperatureGetSensorValue(TEMPERATURE_SENSOR_ID_RADIATOR_START);
			Target_Start_Water_Temperature = TemperatureGetTargetStartWaterTemperature();
			
			// Gas burner control
			if (Radiator_Water_Start_Temperature <= Target_Start_Water_Temperature - CONFIGURATION_GAS_BURNER_TEMPERATURE_HYSTERESIS_LOW) RelayTurnOn(RELAY_ID_GAS_BURNER);
			else if (Radiator_Water_Start_Temperature >= Target_Start_Water_Temperature + CONFIGURATION_GAS_BURNER_TEMPERATURE_HYSTERESIS_HIGH) RelayTurnOff(RELAY_ID_GAS_BURNER);
			
			// Start pump
			RelayTurnOn(RELAY_ID_PUMP);
			
			// Progressively send water to the radiators (assume valve is on the left position, which is set when boiler is stopped)
			MixingValveSetPosition(MIXING_VALVE_POSITION_RIGHT);
			
			// Tell user boiler is running
			LedTurnOff(LED_ID_BOILER_RUNNING_MODE);
		}
		else
		{
			// Make sure burner is stopped
			RelayTurnOff(RELAY_ID_GAS_BURNER);
			
			// Stop pump
			RelayTurnOff(RELAY_ID_PUMP);
			
			// Close radiators water circuit to send cold water only to the gas burner on next run
			MixingValveSetPosition(MIXING_VALVE_POSITION_LEFT);
			
			// Tell user boiler is idle
			LedTurnOn(LED_ID_BOILER_RUNNING_MODE);
		}
		
		// Make the mixing valve moves
		MixingValveTask();
		
		// Tell that controller is still alive
		if (Is_Status_Led_On)
		{
			LedTurnOn(LED_ID_STATUS);
			Is_Status_Led_On = 0;
		}
		else
		{
			LedTurnOff(LED_ID_STATUS);
			Is_Status_Led_On = 1;
		}
		
		// This is a slow regulation process, we can safely wait some time between each loop
		_delay_ms(1000);
	}
}
