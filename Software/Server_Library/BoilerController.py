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
		print("Error : received bad magic number (", hex(receivedByte[0]), ").")
		exit(-1)
		
	# Get the command
	receivedByte = bytearray(_clientSocket.recv(1))
	if receivedByte[0] != fullCommand[1]:
		print("Error : received bad command (", hex(receivedByte[0]), ").")
		exit(-1)
		
	# Get the payload (if any)
	if answerPayloadSize == 0:
		return bytearray()
	return bytearray(_clientSocket.recv(answerPayloadSize))

#--------------------------------------------------------------------------------------------------
# Public functions
#--------------------------------------------------------------------------------------------------
## Create and bind the socket server on the port 1234.
# @note This function should be called only once at the beginning of the program.
def startServer():
	global _serverSocket
	
	# Configure server
	_serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	_serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	_serverSocket.bind(("0.0.0.0", 1234))

## Wait for the client to connect to the server socket.
# @note This function can be called again after the client has disconnected.
def waitForControllerConnection():
	global _serverSocket
	global _clientSocket
	
	# Wait for the client to connect
	_serverSocket.listen(1)
	_clientSocket, clientAddress = _serverSocket.accept()

	# Wait a bit to let the WiFi-to-UART bridge prepare itself for operations
	time.sleep(1);

## Retrieve the version of the currently running firmware.
# @return An integer value corresponding to the firmware version.
def getFirmwareVersion():
	answerPayload = _sendCommand([0], 1)
	return answerPayload[0]

## Retrieve the ADC raw values (but averaged through a moving average) corresponding to external thermistor and start water thermistor.
# @return First value is the external thermistor raw value, second value is the start water thermistor raw value.
# @note This function is dedicated to debug purposes.
def getSensorsRawTemperatures():
	answerPayload = _sendCommand([1], 4)
	externalTemperature = (answerPayload[1] << 8) | answerPayload[0] # Data are sent in little endian
	waterStartTemperature = (answerPayload[3] << 8) | answerPayload[2]
	return externalTemperature, waterStartTemperature

## Retrieve the external and start water thermistors temperatures converted to Celsius units.
# @return First value is the external temperature, second value is the start water temperature.
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

## Retrieve the mixing valve current position.
# @return A string telling the current position.
# @note Valve position is updated only when it stops moving, so the last known position is reported while the valve is moving.
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

## Tell the boiler controller that it must switch from day mode to night mode or vice-versa.
# @param isNightModeEnabled Set to true to switch to night mode, set to false to switch to day mode.
def setNightMode(isNightModeEnabled):
	if isNightModeEnabled:
		_sendCommand([4, 1], 0)
	else:
		_sendCommand([4, 0], 0)

## Retrieve the configured room temperature to reach in the house. This value can be configured using the trimmers or the setDesiredRoomTemperature() command.
# @return An integer number corresponding to the temperature converted to Celsius units.
def getDesiredRoomTemperature():
	answerPayload = _sendCommand([5], 1)
	return answerPayload[0]

## Set the room temperature to reach in the house. Using this function overrides the previously set desired temperature and the temperature specified by the trimmers.
# @param temperature The desired temperature (in Celsius units).
# @note This temperature value is overwritten when using the trimmers.
def setDesiredRoomTemperature(temperature):
	desiredTemperature = bytearray([temperature])
	_sendCommand([6, desiredTemperature])

## Get the trimmers raw ADC values (but averaged through a moving average).
# @return First value is day trimmer value, second value is night trimmer value.
# @note This function is dedicated to debug purposes.
def getTrimmersRawValues():
	answerPayload = _sendCommand([7], 4)
	dayTrimmerValue = (answerPayload[1] << 8) | answerPayload[0] # Data are sent in little endian
	nightTrimmerValue = (answerPayload[3] << 8) | answerPayload[2]
	return dayTrimmerValue, nightTrimmerValue

## Turn the boiler on or off.
# @param isRunning Set to true to start the boiler, set to false to put the boiler in idle mode.
def setBoilerRunningMode(isRunning):
	if isRunning:
		_sendCommand([8, 1], 0)
	else:
		_sendCommand([8, 0], 0)

## Retrieve the clamped value corresponding to the heating curve computation. This is the target start water temperature to reach to obtain the desired room temperature.
# @return The target start water temperature in Celsius units.
# @note This function is dedicated to debug purposes.
def getTargetStartWaterTemperature():
	answerPayload = _sendCommand([9], 1)
	return answerPayload[0]

## Retrieve heating curve settings.
# @return First value is coefficient, second value is parallel shift.
def getHeatingCurveParameters():
	answerPayload = _sendCommand([10], 4)
	heatingCurveCoefficient = ((answerPayload[1] << 8) | answerPayload[0]) / 10.0 # Values are multiplied by ten to improve microcontroller firmware computations, so divide them by ten to get the original value (and cast to float in the same time)
	heatingCurveParallelShift = ((answerPayload[3] << 8) | answerPayload[2]) / 10.0
	return heatingCurveCoefficient, heatingCurveParallelShift
