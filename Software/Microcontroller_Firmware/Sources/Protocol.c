/** @file Protocol.c
 * @see Protocol.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <Configuration.h>
#include <Protocol.h>

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
	if (ProtocolUARTReadByteNoInterrupt() != '\r') return 1;
	if (ProtocolUARTReadByteNoInterrupt() != '\n') return 1;
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void ProtocolInitialize(void)
{
	char String_Command[128];
	
	// Initialize UART module to 115200bit/s, 8-bit data, no parity
	UBRR0H = 0;
	UBRR0L = 1; // Set baud rate to 115200bit/s (formula is UBRR0H:UBRR0L = (Fosc / (16 * Desired_Baud_Rate)) - 1)
	UCSR0A = 0; // Do not double the UART transmission speed (is is useless here and non-doubled speed works better)
	UCSR0C = 0x06; // Select asynchronous UART, disable parity mode, select 1 stop bit, select 8-bit character size
	UCSR0B = 0x18; // Enable reception and transmission TODO enable interrupts later
	
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
}