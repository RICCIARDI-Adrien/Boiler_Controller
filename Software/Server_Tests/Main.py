#!/usr/bin/python

import socket

#--------------------------------------------------------------------------------------------------
# Private constants
#--------------------------------------------------------------------------------------------------
## The port the server will be bound to.
SERVER_PORT = 1234

## The magic number preceding all protocol commands transmission and reception.
PROTOCOL_MAGIC_NUMBER = [0xA5]
## Maximum payload sent by the biggest protocol command.
PROTOCOL_MAXIMUM_PAYLOAD_SIZE = 10
## "Get firmware version" command code.
PROTOCOL_COMMAND_GET_FIRMWARE_VERSION = 0

#--------------------------------------------------------------------------------------------------
# Private functions
#--------------------------------------------------------------------------------------------------
## Send a command and its payload and wait for the answer.
# @param commandAndPayload A byte array composed of the command and the eventual payload.
# @param answerPayloadSize How many bytes of payload will be present in the answer.
# @return A byte array containing the command answer payload.
def sendCommand(commandAndPayload, answerPayloadSize):
	# Add the magic number in front of the data to send to send only one network packet
	fullCommand = bytearray(PROTOCOL_MAGIC_NUMBER)
	fullCommand.extend(bytearray(commandAndPayload))
	
	# Send the whole command
	clientSocket.send(bytearray(fullCommand))
	
	# Get the answer
	receivedByte = bytearray(clientSocket.recv(1))
	if receivedByte[0] != fullCommand[0]:
		print "Error : received bad magic number (", hex(receivedByte[0]), ")."
		exit(-1)
		
	# Get the command
	receivedByte = bytearray(clientSocket.recv(1))
	if receivedByte[0] != fullCommand[1]:
		print "Error : received bad command (", hex(receivedByte[0]), ")."
		exit(-1)
		
	# Get the payload (if any)
	if answerPayloadSize == 0:
		return bytearray()
	return bytearray(clientSocket.recv(answerPayloadSize))

#--------------------------------------------------------------------------------------------------
# Entry point
#--------------------------------------------------------------------------------------------------
# Configure server
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serverSocket.bind(("0.0.0.0", SERVER_PORT))
print "Waiting for client..."

# Wait for the client to connect
serverSocket.listen(1)
clientSocket, clientAddress = serverSocket.accept()
print "Client connected with address", clientAddress

# Get firmware version
version = sendCommand([PROTOCOL_COMMAND_GET_FIRMWARE_VERSION], 1)
print "Firmware version :", version[0]