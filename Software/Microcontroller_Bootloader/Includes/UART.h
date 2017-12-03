/** @file UART.h
 * Simple no-interrupt UART driver.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Configure the UART module for 115200 bit/s, 8 data bits, no parity. */
void UARTInitialize(void);

/** Read a byte from the serial port without relying on interrupt handler.
 * @return The read value.
 */
unsigned char UARTReadByte(void);

/** Write a byte to the serial port without relying on interrupt handler.
 * @param Byte The byte value.
 */
void UARTWriteByte(unsigned char Byte);

/** Write a zero-terminated string to the serial port without relying on interrupt handler.
 * @param String The string to write (terminating zero is not written).
 */
void UARTWriteString(char *String);

#endif
