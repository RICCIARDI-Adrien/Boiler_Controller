/** @file Protocol.h
 * Use an ESP8266 through the microcontroller UART to communicate with the server.
 * @author Adrien RICCIARDI
 */
#ifndef H_PROTOCOL_H
#define H_PROTOCOL_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Initialize the UART module used to communicate with the ESP8266 and connect the ESP8266 to the network. */
void ProtocolInitialize(void);

#endif