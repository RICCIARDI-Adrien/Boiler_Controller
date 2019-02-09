/** @file Protocol.c
 * @see Protocol.h for description.
 * @author Adrien RICCIARDI
 */
#include <ADC.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Configuration.h>
#include <Mixing_Valve.h>
#include <Protocol.h>
#include <Temperature.h>
#include <util/delay.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The magic number preceding all received and sent commands. */
#define PROTOCOL_MAGIC_NUMBER 0xA5

/** The biggest command payload size. */
#define PROTOCOL_PAYLOAD_MAXIMUM_SIZE 6 // TODO set when all commands are decided

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** All reception and transmission state machine states. */
typedef enum
{
	PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER,
	PROTOCOL_STATE_RECEIVE_COMMAND,
	PROTOCOL_STATE_RECEIVE_PAYLOAD,
	PROTOCOL_STATE_TRANSMIT_COMMAND,
	PROTOCOL_STATE_TRANSMIT_PAYLOAD,
	PROTOCOL_STATES_COUNT
} TProtocolState;

/** All known commands. */
typedef enum
{
	PROTOCOL_COMMAND_GET_FIRMWARE_VERSION,
	PROTOCOL_COMMAND_GET_SENSORS_RAW_TEMPERATURES,
	PROTOCOL_COMMAND_GET_SENSORS_CELSIUS_TEMPERATURES,
	PROTOCOL_COMMAND_GET_MIXING_VALVE_POSITION,
	PROTOCOL_COMMAND_SET_NIGHT_MODE,
	PROTOCOL_COMMAND_GET_DESIRED_ROOM_TEMPERATURES,
	PROTOCOL_COMMAND_SET_DESIRED_ROOM_TEMPERATURES,
	PROTOCOL_COMMAND_GET_TRIMMERS_RAW_VALUES,
	PROTOCOL_COMMAND_GET_BOILER_RUNNING_MODE,
	PROTOCOL_COMMAND_SET_BOILER_RUNNING_MODE,
	PROTOCOL_COMMAND_GET_TARGET_START_WATER_TEMPERATURE,
	PROTOCOL_COMMAND_GET_HEATING_CURVE_PARAMETERS,
	PROTOCOL_COMMAND_SET_HEATING_CURVE_PARAMETERS,
	PROTOCOL_COMMANDS_COUNT
} TProtocolCommand;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The current protocol state machine state. */
static TProtocolState Protocol_State = PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER;

/** The last received command, or the command to transmit. */
static TProtocolCommand Protocol_Command;
/** The command payload content. */
static unsigned char Protocol_Command_Payload_Buffer[PROTOCOL_PAYLOAD_MAXIMUM_SIZE];
/** The command payload index, indicating where to store the next received byte or what byte to transmit next. */
static unsigned char Protocol_Command_Payload_Index;
/** The command payload size in bytes (as well in reception as in transmission mode). */
static unsigned char Protocol_Command_Payload_Size;

/** Tell whether the boiler is currently running or idle. */
static unsigned char Protocol_Is_Boiler_Running = 1; // Automatically enable the boiler on power on

/** Tell if this is night or day. */
static unsigned char Protocol_Is_Night_Mode_Enabled = 0;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Read a byte from the serial port without relying on interrupt handler.
 * @return The read value.
 */
static unsigned char ProtocolUARTReadByteNoInterrupt(void)
{
	// Wait for a byte to be received
	while (!(UCSR0A & 0x80));
	
	// Get the byte
	return UDR0;
}

/** Write a byte to the serial port without relying on interrupt handler.
 * @param Byte The byte value.
 */
static void ProtocolUARTWriteByteNoInterrupt(unsigned char Byte)
{
	// Send the byte
	UDR0 = Byte;
	
	// Wait for the byte to be transmitted
	while (!(UCSR0A & 0x40));
	UCSR0A |= 0x40; // Clear "transmit complete" flag
}

/** Write a zero-terminated string to the serial port without relying on interrupt handler.
 * @param String The string to write (terminating zero is not written).
 */
static void ProtocolUARTWriteStringNoInterrupt(char *String)
{
	while (*String != 0)
	{
		ProtocolUARTWriteByteNoInterrupt(*String);
		String++;
	}
}

/** Wait for a specific string from the ESP8266 module (terminating "\r\n" are automatically added, no need to provide them in string).
 * @param String_Success_Answer The expected string in case of success.
 * @param String_Error_Answer The expected string in case of error.
 * @return 0 if the error string was received,
 * @return 1 if the success string was received.
 * @warning This function never returns if none of the strings are received.
 */
static unsigned char ProtocolESP8266IsCommandSuccessful(char *String_Success_Answer, char *String_Error_Answer)
{
	unsigned char Received_Byte, Success_String_Index = 0, Error_String_Index = 0, Is_Success_String_Found = 0;
	
	// Receive characters until one of the two strings is fully detected
	while (1)
	{
		Received_Byte = ProtocolUARTReadByteNoInterrupt();
		
		// Is this character matching the success string current character ?
		if (Received_Byte != String_Success_Answer[Success_String_Index]) Success_String_Index = 0;
		else
		{
			Success_String_Index++;
			
			// Is the success string fully received ?
			if (String_Success_Answer[Success_String_Index] == 0)
			{
				Is_Success_String_Found = 1;
				break;
			}
		}
		
		// Is this character matching the error string current character ?
		if (Received_Byte != String_Error_Answer[Error_String_Index]) Error_String_Index = 0;
		else
		{
			Error_String_Index++;
			
			// Is the error string fully received ?
			if (String_Error_Answer[Error_String_Index] == 0)
			{
				Is_Success_String_Found = 0;
				break;
			}
		}
	}
	
	// Wait for terminating "\r\n"
	while (ProtocolUARTReadByteNoInterrupt() != '\r');
	while (ProtocolUARTReadByteNoInterrupt() != '\n');
	
	return Is_Success_String_Found;
}

/** Execute a fully received command. */
static void ProtocolExecuteCommand(void)
{
	unsigned short *Pointer_Word;
	
	switch (Protocol_Command)
	{
		case PROTOCOL_COMMAND_GET_FIRMWARE_VERSION:
			Protocol_Command_Payload_Buffer[0] = CONFIGURATION_FIRMWARE_VERSION;
			Protocol_Command_Payload_Size = 1;
			break;
			
		case PROTOCOL_COMMAND_GET_SENSORS_RAW_TEMPERATURES:
			Pointer_Word = (unsigned short *) Protocol_Command_Payload_Buffer;
			*Pointer_Word = ADCGetLastSampledValue(ADC_CHANNEL_ID_OUTSIDE_THERMISTOR);
			Pointer_Word++;
			*Pointer_Word = ADCGetLastSampledValue(ADC_CHANNEL_ID_RADIATOR_START_THERMISTOR);
			Protocol_Command_Payload_Size = 4;
			break;
			
		case PROTOCOL_COMMAND_GET_SENSORS_CELSIUS_TEMPERATURES:
			Protocol_Command_Payload_Buffer[0] = (unsigned char) TemperatureGetSensorValue(TEMPERATURE_SENSOR_ID_OUTSIDE);
			Protocol_Command_Payload_Buffer[1] = (unsigned char) TemperatureGetSensorValue(TEMPERATURE_SENSOR_ID_RADIATOR_START);
			Protocol_Command_Payload_Size = 2;
			break;
			
		case PROTOCOL_COMMAND_GET_MIXING_VALVE_POSITION:
			Protocol_Command_Payload_Buffer[0] = MixingValveGetPosition();
			Protocol_Command_Payload_Size = 1;
			break;
			
		case PROTOCOL_COMMAND_SET_NIGHT_MODE:
			Protocol_Is_Night_Mode_Enabled = Protocol_Command_Payload_Buffer[0];
			Protocol_Command_Payload_Size = 0;
			break;
			
		case PROTOCOL_COMMAND_GET_DESIRED_ROOM_TEMPERATURES:
			TemperatureGetDesiredRoomTemperatures((signed char *) &Protocol_Command_Payload_Buffer[0], (signed char *) &Protocol_Command_Payload_Buffer[1]);
			Protocol_Command_Payload_Size = 2;
			break;
			
		case PROTOCOL_COMMAND_SET_DESIRED_ROOM_TEMPERATURES:
			TemperatureSetDesiredRoomTemperatures((signed char) Protocol_Command_Payload_Buffer[0], (signed char) Protocol_Command_Payload_Buffer[1]);
			Protocol_Command_Payload_Size = 0;
			break;
			
		case PROTOCOL_COMMAND_GET_TRIMMERS_RAW_VALUES:
			Pointer_Word = (unsigned short *) Protocol_Command_Payload_Buffer;
			*Pointer_Word = ADCGetLastSampledValue(ADC_CHANNEL_ID_DAY_TRIMMER);
			Pointer_Word++;
			*Pointer_Word = ADCGetLastSampledValue(ADC_CHANNEL_ID_NIGHT_TRIMMER);
			Protocol_Command_Payload_Size = 4;
			break;
			
		case PROTOCOL_COMMAND_GET_BOILER_RUNNING_MODE:
			Protocol_Command_Payload_Buffer[0] = Protocol_Is_Boiler_Running;
			Protocol_Command_Payload_Size = 1;
			break;
			
		case PROTOCOL_COMMAND_SET_BOILER_RUNNING_MODE:
			Protocol_Is_Boiler_Running = Protocol_Command_Payload_Buffer[0];
			Protocol_Command_Payload_Size = 0;
			break;
			
		case PROTOCOL_COMMAND_GET_TARGET_START_WATER_TEMPERATURE:
			Protocol_Command_Payload_Buffer[0] = TemperatureGetTargetStartWaterTemperature();
			Protocol_Command_Payload_Size = 1;
			break;
			
		case PROTOCOL_COMMAND_GET_HEATING_CURVE_PARAMETERS:
			TemperatureGetHeatingCurveParameters((unsigned short *) &Protocol_Command_Payload_Buffer[0], (unsigned short *) &Protocol_Command_Payload_Buffer[2]);
			Protocol_Command_Payload_Size = 4;
			break;
			
		case PROTOCOL_COMMAND_SET_HEATING_CURVE_PARAMETERS:
			Pointer_Word = (unsigned short *) Protocol_Command_Payload_Buffer;
			TemperatureSetHeatingCurveParameters(Pointer_Word[0], Pointer_Word[1]);
			Protocol_Command_Payload_Size = 0;
			break;
			
		// Unknown command, should not get here
		default:
			break;
	}
	
	// Send command answer
	Protocol_State = PROTOCOL_STATE_TRANSMIT_COMMAND;
	Protocol_Command_Payload_Index = 0;
	UDR0 = PROTOCOL_MAGIC_NUMBER;
}

/** Handle UART reception interrupts. */
ISR(USART_RX_vect)
{
	static unsigned char Received_Command_Payload[PROTOCOL_COMMANDS_COUNT] =
	{
		0, // PROTOCOL_COMMAND_GET_FIRMWARE_VERSION
		0, // PROTOCOL_COMMAND_GET_SENSORS_RAW_TEMPERATURES
		0, // PROTOCOL_COMMAND_GET_SENSORS_CELSIUS_TEMPERATURES
		0, // PROTOCOL_COMMAND_GET_MIXING_VALVE_POSITION
		1, // PROTOCOL_COMMAND_SET_NIGHT_MODE
		0, // PROTOCOL_COMMAND_GET_DESIRED_ROOM_TEMPERATURES
		2, // PROTOCOL_COMMAND_SET_DESIRED_ROOM_TEMPERATURES
		0, // PROTOCOL_COMMAND_GET_TRIMMERS_RAW_VALUES
		0, // PROTOCOL_COMMAND_GET_BOILER_RUNNING_MODE
		1, // PROTOCOL_COMMAND_SET_BOILER_RUNNING_MODE
		0, // PROTOCOL_COMMAND_GET_TARGET_START_WATER_TEMPERATURE
		0, // PROTOCOL_COMMAND_GET_HEATING_CURVE_PARAMETERS
		4 // PROTOCOL_COMMAND_SET_HEATING_CURVE_PARAMETERS
	};
	unsigned char Byte;
	
	// Get the received byte
	Byte = UDR0;
	
	switch (Protocol_State)
	{
		case PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER:
			if (Byte == PROTOCOL_MAGIC_NUMBER) Protocol_State = PROTOCOL_STATE_RECEIVE_COMMAND;
			break;
			
		case PROTOCOL_STATE_RECEIVE_COMMAND:
			Protocol_Command = Byte;
			// Make sure it is a known command
			if (Protocol_Command >= PROTOCOL_COMMANDS_COUNT)
			{
				Protocol_State = PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER; // Abort current command reception
				break;
			}
			// Determine how many bytes of payload to receive
			Protocol_Command_Payload_Size = Received_Command_Payload[Protocol_Command];
			// Execute the command if there is no payload to receive
			if (Protocol_Command_Payload_Size == 0) ProtocolExecuteCommand();
			else
			{
				Protocol_Command_Payload_Index = 0; // Start filling the payload buffer from the beginning
				Protocol_State = PROTOCOL_STATE_RECEIVE_PAYLOAD;
			}
			break;
			
		case PROTOCOL_STATE_RECEIVE_PAYLOAD:
			// Receive next byte
			Protocol_Command_Payload_Buffer[Protocol_Command_Payload_Index] = Byte;
			Protocol_Command_Payload_Index++;
			
			// Execute command if the payload is fully received
			if (Protocol_Command_Payload_Index == Protocol_Command_Payload_Size) ProtocolExecuteCommand();
			break;
		
		// Unknown state, do nothing
		default:
			break;
	}
}

/** Handle UART transmission interrupts. */
ISR(USART_TX_vect)
{
	switch (Protocol_State)
	{
		case PROTOCOL_STATE_TRANSMIT_COMMAND:
			UDR0 = Protocol_Command;
			Protocol_State = PROTOCOL_STATE_TRANSMIT_PAYLOAD; // There is no special case for commands that do not provide payload in order to wait for the transmission interrupt to happen, this way we are sure that the byte was transmitted and we can safely go to another state of the state machine
			break;
			
		case PROTOCOL_STATE_TRANSMIT_PAYLOAD:
			// Wait for another command if this one's answer has been fully transmitted
			if (Protocol_Command_Payload_Index == Protocol_Command_Payload_Size) Protocol_State = PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER;
			else
			{
				UDR0 = Protocol_Command_Payload_Buffer[Protocol_Command_Payload_Index];
				Protocol_Command_Payload_Index++;
			}
			break;
			
		// Unknown state, do nothing
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
unsigned char ProtocolInitialize(void)
{
	// Initialize UART module to 115200bit/s, 8-bit data, no parity
	UBRR0H = 0;
	UBRR0L = 1; // Set baud rate to 115200bit/s (formula is UBRR0H:UBRR0L = (Fosc / (16 * Desired_Baud_Rate)) - 1)
	UCSR0A = 0; // Do not double the UART transmission speed (is is useless here and non-doubled speed works better)
	UCSR0C = 0x06; // Select asynchronous UART, disable parity mode, select 1 stop bit, select 8-bit character size
	UCSR0B = 0x18; // Enable reception and transmission
	
	// Reset the module in case the microcontroller rebooted (due to firmware programming). This reset sequence does not harm if the ESP8266 is powered at the same time the microcontroller is (for instance when powering the board), WiFi will just take some more seconds to connect
	ProtocolUARTWriteStringNoInterrupt("+++"); // Exit from "transparent bridge mode"
	_delay_ms(2000); // Wait at least 1 second for the "+++" sequence to be validated (see https://en.wikipedia.org/wiki/Hayes_command_set)
	ProtocolUARTWriteStringNoInterrupt("AT+RST\r\n"); // Reset module
	
	// ESP8266 will send a lot of data with a bad baud rate followed by the string "ready"
	ProtocolESP8266IsCommandSuccessful("ready", "THIS STRING CAN'T BE FOUND");
	
	// Set WiFi mode to access point + station, it's mandatory for the transparent mode to work
	ProtocolUARTWriteStringNoInterrupt("AT+CWMODE_CUR=3\r\n");
	if (!ProtocolESP8266IsCommandSuccessful("\r\nOK", "\r\nERROR")) return 0;
	
	// Try to connect to access point
	ProtocolUARTWriteStringNoInterrupt("AT+CWJAP_CUR=\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_SSID);
	ProtocolUARTWriteStringNoInterrupt("\",\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_PASSWORD);
	ProtocolUARTWriteStringNoInterrupt("\"\r\n");
	if (!ProtocolESP8266IsCommandSuccessful("\r\n\r\nOK", "\r\n\r\nFAIL")) return 0; // A fully matching string must be provided because the string comparison function does not recheck the current character with the received one when the index has been reset (for the sake of simplicity)
	
	// Try to connect to server
	ProtocolUARTWriteStringNoInterrupt("AT+CIPSTART=\"TCP\",\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_SERVER_ADDRESS);
	ProtocolUARTWriteStringNoInterrupt("\",");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_SERVER_PORT);
	ProtocolUARTWriteStringNoInterrupt("\r\n");
	if (!ProtocolESP8266IsCommandSuccessful("\r\n\r\nOK", "\r\nERROR\r\nCLOSED")) return 0;
	
	// Set connection mode to "transparent bridge" to directly transmit what is written to the ESP8266 UART
	ProtocolUARTWriteStringNoInterrupt("AT+CIPMODE=1\r\n");
	if (!ProtocolESP8266IsCommandSuccessful("\r\nOK", "\r\nERROR")) return 0;
	
	ProtocolUARTWriteStringNoInterrupt("AT+CIPSEND\r\n");
	if (!ProtocolESP8266IsCommandSuccessful("\r\nOK", "\r\nERROR")) return 0;
	
	// Enable interrupts now that the WiFi bridge has been initialized
	PROTOCOL_ENABLE_INTERRUPTS();
	
	return 1;
}

unsigned char ProtocolIsBoilerRunning(void)
{
	return Protocol_Is_Boiler_Running;
}

unsigned char ProtocolIsNightModeEnabled(void)
{
	return Protocol_Is_Night_Mode_Enabled;
}
