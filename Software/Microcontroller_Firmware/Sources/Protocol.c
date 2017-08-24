/** @file Protocol.c
 * @see Protocol.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Configuration.h>
#include <Protocol.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The magic number preceding all received and sent commands. */
#define PROTOCOL_MAGIC_NUMBER 0xA5

/** The biggest command payload size. */
#define PROTOCOL_PAYLOAD_MAXIMUM_SIZE 3 // TODO set when all commands are decided

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
 * @param String_Expected_Answer The expected string.
 */
static void ProtocolESP8266WaitForAnswer(char *String_Expected_Answer)
{
	unsigned char Received_Byte, i = 0;
	
	// Wait for all string characters
	while (String_Expected_Answer[i] != 0)
	{
		Received_Byte = ProtocolUARTReadByteNoInterrupt();
		if (Received_Byte != String_Expected_Answer[i]) i = 0;
		else i++;
	}
	
	// Wait for terminating "\r\n"
	while (ProtocolUARTReadByteNoInterrupt() != '\r');
	while (ProtocolUARTReadByteNoInterrupt() != '\n');;
}

/** Execute a fully received command. */
static void ProtocolExecuteCommand(void)
{
	switch (Protocol_Command)
	{
		case PROTOCOL_COMMAND_GET_FIRMWARE_VERSION:
			Protocol_Command_Payload_Buffer[0] = CONFIGURATION_FIRMWARE_VERSION;
			Protocol_Command_Payload_Size = 1;
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
			// Execute command if the payload is fully received
			if (Protocol_Command_Payload_Index == Protocol_Command_Payload_Size) ProtocolExecuteCommand();
			else
			{
				Protocol_Command_Payload_Buffer[Protocol_Command_Payload_Index] = UDR0;
				Protocol_Command_Payload_Index++;
			}
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
void ProtocolInitialize(void)
{
	// Initialize UART module to 115200bit/s, 8-bit data, no parity
	UBRR0H = 0;
	UBRR0L = 1; // Set baud rate to 115200bit/s (formula is UBRR0H:UBRR0L = (Fosc / (16 * Desired_Baud_Rate)) - 1)
	UCSR0A = 0; // Do not double the UART transmission speed (is is useless here and non-doubled speed works better)
	UCSR0C = 0x06; // Select asynchronous UART, disable parity mode, select 1 stop bit, select 8-bit character size
	UCSR0B = 0x18; // Enable reception and transmission
	
	// ESP8266 will send a lot of binary data followed by the string "ready"
	ProtocolESP8266WaitForAnswer("ready");
	
	// Set WiFi mode to access point + station, it's mandatory for the transparent mode to work
	ProtocolUARTWriteStringNoInterrupt("AT+CWMODE_CUR=3\r\n");
	ProtocolESP8266WaitForAnswer("\r\nOK");
	
	// Try to connect to access point
	ProtocolUARTWriteStringNoInterrupt("AT+CWJAP_CUR=\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_SSID);
	ProtocolUARTWriteStringNoInterrupt("\",\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_ACCESS_POINT_PASSWORD);
	ProtocolUARTWriteStringNoInterrupt("\"\r\n");
	ProtocolESP8266WaitForAnswer("WIFI CONNECTED");
	ProtocolESP8266WaitForAnswer("WIFI GOT IP");
	ProtocolESP8266WaitForAnswer("\r\nOK");
	
	// Try to connect to server
	ProtocolUARTWriteStringNoInterrupt("AT+CIPSTART=\"TCP\",\"");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_SERVER_ADDRESS);
	ProtocolUARTWriteStringNoInterrupt("\",");
	ProtocolUARTWriteStringNoInterrupt(CONFIGURATION_PROTOCOL_WIFI_SERVER_PORT);
	ProtocolUARTWriteStringNoInterrupt("\r\n");
	ProtocolESP8266WaitForAnswer("CONNECT");
	ProtocolESP8266WaitForAnswer("\r\nOK");
	
	// Set connection mode to "transparent bridge" to directly transmit what is written to the ESP8266 UART
	ProtocolUARTWriteStringNoInterrupt("AT+CIPMODE=1\r\n");
	ProtocolESP8266WaitForAnswer("\r\nOK");
	
	ProtocolUARTWriteStringNoInterrupt("AT+CIPSEND\r\n");
	ProtocolESP8266WaitForAnswer("\r\nOK");
	
	// Enable interrupts now that the WiFi bridge has been initialized
	PROTOCOL_ENABLE_INTERRUPTS();
}
