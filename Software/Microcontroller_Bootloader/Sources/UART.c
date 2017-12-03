/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <avr/io.h>
#include <UART.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void UARTInitialize(void)
{
	// Initialize UART module to 115200bit/s, 8-bit data, no parity
	UBRR0H = 0;
	UBRR0L = 1; // Set baud rate to 115200bit/s (formula is UBRR0H:UBRR0L = (Fosc / (16 * Desired_Baud_Rate)) - 1)
	UCSR0A = 0; // Do not double the UART transmission speed (is is useless here and non-doubled speed works better)
	UCSR0C = 0x06; // Select asynchronous UART, disable parity mode, select 1 stop bit, select 8-bit character size
	UCSR0B = 0x18; // Enable reception and transmission
}

unsigned char UARTReadByte(void)
{
	// Wait for a byte to be received
	while (!(UCSR0A & 0x80));
	
	// Get the byte
	return UDR0;
}

void UARTWriteByte(unsigned char Byte)
{
	// Send the byte
	UDR0 = Byte;
	
	// Wait for the byte to be transmitted
	while (!(UCSR0A & 0x40));
	UCSR0A |= 0x40; // Clear "transmit complete" flag
}

void UARTWriteString(char *String)
{
	while (*String != 0)
	{
		UARTWriteByte(*String);
		String++;
	}
}
