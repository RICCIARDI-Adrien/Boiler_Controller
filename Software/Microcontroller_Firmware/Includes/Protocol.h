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
/** Initialize the UART module used to communicate with the ESP8266 and connect the ESP8266 to the network.
 * @return 0 if the connection succeeded and WiFi is operational,
 * @return 1 if an error occurred and WiFi won't work.
 */
unsigned char ProtocolInitialize(void);

/** Tell whether the boiler is running or idle.
 * @return 0 if the boiler is idle,
 * @return 1 if the boiler is running.
 */
unsigned char ProtocolIsBoilerRunning(void);

/** Tell whether this is night or day. This allows to select different temperature settings according to the time.
 * @return 0 if this is day,
 * @return 1 if this is night.
 */
unsigned char ProtocolIsNightModeEnabled(void);

#endif
