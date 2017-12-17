#!/usr/bin/python
## @file Main.py
## Send commands to the controller board to check if everything is fine.
## @author Adrien RICCIARDI
import socket
import struct
import time

#--------------------------------------------------------------------------------------------------
# Private constants
#--------------------------------------------------------------------------------------------------
## The port the server will be bound to.
SERVER_PORT = 1234

## The magic number preceding all protocol commands transmission and reception.
PROTOCOL_MAGIC_NUMBER = [0xA5]
## "Get firmware version" command code.
PROTOCOL_COMMAND_GET_FIRMWARE_VERSION = 0
## "Get raw temperatures" command code.
PROTOCOL_COMMAND_GET_RAW_TEMPERATURES = 1
## "Get Celsius temperatures" command code.
PROTOCOL_COMMAND_GET_CELSIUS_TEMPERATURES = 2
# "Get mixing valve position" command code.
PROTOCOL_COMMAND_GET_MIXING_VALVE_POSITION = 3

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

time.sleep(1);

# Get firmware version
answerPayload = sendCommand([PROTOCOL_COMMAND_GET_FIRMWARE_VERSION], 1)
firmwareVersion = answerPayload[0]
print "Firmware version :", firmwareVersion
if firmwareVersion != 1:
	print "Error : bad firmware version."
	exit(-1)

# Get raw temperatures
answerPayload = sendCommand([PROTOCOL_COMMAND_GET_RAW_TEMPERATURES], 4)
externalTemperature = (answerPayload[1] << 8) | answerPayload[0] # Data are sent in little endian
startTemperature = (answerPayload[3] << 8) | answerPayload[2]
print "Raw external temperature :", hex(externalTemperature), ", raw start temperature :", hex(startTemperature)

# Get Celsius temperatures
answerPayload = sendCommand([PROTOCOL_COMMAND_GET_CELSIUS_TEMPERATURES], 3)
externalTemperature = answerPayload[0]
startTemperature = answerPayload[1]
returnTemperature = answerPayload[2]
# Python is so shitty that it does not even know how to do a cast as simple as unsigned char to char
if externalTemperature > 127:
	externalTemperature = -1 * (256 - externalTemperature)
if startTemperature > 127:
	startTemperature = -1 * (256 - startTemperature)
if returnTemperature > 127:
	returnTemperature = -1 * (256 - returnTemperature)
print "Celsius external temperature :", externalTemperature, ", Celsius start temperature :", startTemperature, ", Celsius return temperature :", returnTemperature

# Get mixing valve position
answerPayload = sendCommand([PROTOCOL_COMMAND_GET_MIXING_VALVE_POSITION], 1)
if answerPayload[0] == 0:
	valvePosition = "left"
elif answerPayload[0] == 1:
	valvePosition = "center"
elif answerPayload[0] == 2:
	valvePosition = "right"
else:
	valvePosition = "unknown"
print "Mixing valve position :", valvePosition
if valvePosition == "unknown":
	print "Error : unknown valve position."
	exit(-1)

print "\033[32mAll tests succeeded.\033[0m"
