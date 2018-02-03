#!/usr/bin/python
## @file Main.py
## Send commands to the controller board to check if everything is fine.
## @author Adrien RICCIARDI
import socket
import struct
import sys
import time

# Module to import location must be explicitly specified if module is not located in the script same directory
sys.path.append('../Server_Library')
import BoilerController

#--------------------------------------------------------------------------------------------------
# Entry point
#--------------------------------------------------------------------------------------------------
BoilerController.startServer()

while True:
	# Create socket for the boiler controller to connect to
	print "Waiting for controller board connection..."
	BoilerController.waitForControllerConnection()
	print "Controller board connected."
	
	try:
		while True:
			# Display firmware version
			firmwareVersion = BoilerController.getFirmwareVersion()
			print "Firmware version :", firmwareVersion

			# Display raw temperatures
			externalTemperature, waterStartTemperature = BoilerController.getSensorsRawTemperatures()
			print "Raw external temperature :", hex(externalTemperature), ", raw water start temperature :", hex(waterStartTemperature)

			# Display Celsius temperatures
			externalTemperature, waterStartTemperature = BoilerController.getSensorsCelsiusTemperatures()
			print "Celsius external temperature :", externalTemperature, ", Celsius water start temperature :", waterStartTemperature

			# Display mixing valve current position
			valvePosition = BoilerController.getMixingValvePosition()
			print "Mixing valve current position :", valvePosition
			
			# Display desired temperature
			temperature = BoilerController.getDesiredRoomTemperature()
			print "Desired room temperature :", valvePosition
			
			dayTrimmerValue, nightTrimmerValue = BoilerController.getTrimmersRawValues()
			print "Raw day trimmer value :", hex(dayTrimmerValue), ", raw night trimmer value :", hex(nightTrimmerValue)
	except:
		print "Controller board disconnected."
	
