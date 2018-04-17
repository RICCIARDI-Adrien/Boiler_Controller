#!/usr/bin/python3
## @file ConfigureHeatingCurve.py
## Configure the heating curve parameters.
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

# Create socket for the boiler controller to connect to
print("Waiting for controller board connection...")
BoilerController.waitForControllerConnection()
print("Controller board connected.")

print("Setting heating curve parameters...");
BoilerController.setHeatingCurveParameters(1.8, 20)

print("Reading configured heating curve...")
heatingCurveCoefficient, heatingCurveParallelShift = BoilerController.getHeatingCurveParameters()
print("Coefficient :", heatingCurveCoefficient, ", parallel shift :", heatingCurveParallelShift)
