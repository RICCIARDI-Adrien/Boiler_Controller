## @file BoilerController.py
## Allow to send command through WiFi to the boiler controller microcontroller.
## @author Adrien RICCIARDI
import socket
import struct
import time

#--------------------------------------------------------------------------------------------------
# Private functions
#--------------------------------------------------------------------------------------------------
## Send a command and its payload and wait for the answer.
# @param commandAndPayload A byte array composed of the command and the eventual payload.
# @param answerPayloadSize How many bytes of payload will be present in the answer.
# @return A byte array containing the command answer payload.
def _sendCommand(commandAndPayload, answerPayloadSize):
	global _clientSocket
	
	# Add the magic number in front of the data to send in order to send only one network packet
	fullCommand = bytearray([0xA5])
	fullCommand.extend(bytearray(commandAndPayload))
	
	# Send the whole command
	_clientSocket.send(bytearray(fullCommand))
	
	# Get the answer
	receivedByte = bytearray(_clientSocket.recv(1))
	if receivedByte[0] != fullCommand[0]:
		print "Error : received bad magic number (", hex(receivedByte[0]), ")."
		exit(-1)
		
	# Get the command
	receivedByte = bytearray(_clientSocket.recv(1))
	if receivedByte[0] != fullCommand[1]:
		print "Error : received bad command (", hex(receivedByte[0]), ")."
		exit(-1)
		
	# Get the payload (if any)
	if answerPayloadSize == 0:
		return bytearray()
	return bytearray(_clientSocket.recv(answerPayloadSize))

#--------------------------------------------------------------------------------------------------
# Public functions
#--------------------------------------------------------------------------------------------------
def startServer():
	global _serverSocket
	
	# Configure server
	_serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	_serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	_serverSocket.bind(("0.0.0.0", 1234))

def waitForControllerConnection():
	global _serverSocket
	global _clientSocket
	
	# Wait for the client to connect
	_serverSocket.listen(1)
	_clientSocket, clientAddress = _serverSocket.accept()

	# Wait a bit to let the WiFi-to-UART bridge prepare itself for operations
	time.sleep(1);

def getFirmwareVersion():
	answerPayload = _sendCommand([0], 1)
	return answerPayload[0]

def getSensorsRawTemperatures():
	answerPayload = _sendCommand([1], 4)
	externalTemperature = (answerPayload[1] << 8) | answerPayload[0] # Data are sent in little endian
	waterStartTemperature = (answerPayload[3] << 8) | answerPayload[2]
	return externalTemperature, waterStartTemperature

def getSensorsCelsiusTemperatures():
	answerPayload = _sendCommand([2], 2)
	externalTemperature = answerPayload[0]
	waterStartTemperature = answerPayload[1]
	
	# Python is so shitty that it does not even know how to do a cast as simple as unsigned char to char
	if externalTemperature > 127:
		externalTemperature = -1 * (256 - externalTemperature)
	if waterStartTemperature > 127:
		waterStartTemperature = -1 * (256 - waterStartTemperature)
	return externalTemperature, waterStartTemperature
	
def getMixingValvePosition():
	answerPayload = _sendCommand([3], 1)
	if answerPayload[0] == 0:
		valvePosition = "left"
	elif answerPayload[0] == 1:
		valvePosition = "center"
	elif answerPayload[0] == 2:
		valvePosition = "right"
	else:
		valvePosition = "unknown"
	return valvePosition

def setNightMode(isNightModeEnabled):
	if isNightModeEnabled:
		_sendCommand([4, 1], 0)
	else:
		_sendCommand([4, 0], 0)
	
def getDesiredRoomTemperature():
	answerPayload = _sendCommand([5], 1)
	return answerPayload[0]

def setDesiredRoomTemperature(temperature):
	desiredTemperature = bytearray(temperature)
	_sendCommand([6, desiredTemperature])

def getTrimmersRawValues():
	answerPayload = _sendCommand([7], 4)
	dayTrimmerValue = (answerPayload[1] << 8) | answerPayload[0] # Data are sent in little endian
	nightTrimmerValue = (answerPayload[3] << 8) | answerPayload[2]
	return dayTrimmerValue, nightTrimmerValue

def setBoilerRunningMode(isRunning):
	if isRunning:
		_sendCommand([8, 1], 0)
	else:
		_sendCommand([8, 0], 0)

def getTargetStartWaterTemperature():
	answerPayload = _sendCommand([9], 1)
	return answerPayload[0]
