#!/usr/bin/python3
import board
import busio
import adafruit_tsl2591

# Initialize I2C Interface
i2c = busio.I2C(board.SCL, board.SDA)
sensor = None


def init():
	"""Initialize TSL2591 Lux Sensor

	Returns:
		int: 0 = OK | -1 = ERR
	"""
	global sensor
	try:
		sensor = adafruit_tsl2591.TSL2591(i2c)
		return 0
	except:
		print("Failed to Initialize Sensor")
		return -1

def lux():
	try:
		return sensor.lux
	except Exception as e:
		print(e)
		return -1

def visible():
	try:
		return sensor.visible
	except:
		return -1

def infrared():
	try:
		return sensor.infrared
	except:
		return -1


