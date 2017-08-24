/** @file Protocol.h
 * Use an ESP8266 through the microcontroller UART to communicate with the server.
 * @author Adrien RICCIARDI
 */
#ifndef H_PROTOCOL_H
#define H_PROTOCOL_H

#include <avr/io.h>

//-------------------------------------------------------------------------------------------------
// Constants and macros
//-------------------------------------------------------------------------------------------------
/** Enable UART interrupts. */
#define PROTOCOL_ENABLE_INTERRUPTS() UCSR0B |= 0xC0 // Enable "receive complete" and "transmit complete" interrupts
/** Disable UART interrupts. */
#define PROTOCOL_DISABLE_INTERRUPTS() UCSR0B &= ~0xC0 // Disable "receive complete" and "transmit complete" interrupts

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Initialize the UART module used to communicate with the ESP8266 and connect the ESP8266 to the network. */
void ProtocolInitialize(void);

#endif
