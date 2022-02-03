#!/usr/bin/python3
import board
import digitalio
import busio
import time
import adafruit_bme280

# create SPI interface object
spi = busio.SPI(board.SCK, board.MOSI, board.MISO)
bme_cs = digitalio.DigitalInOut(board.D5)
bme280 = None

# Initialize bme280 
def init():
	"""Initialization function for BME280
	   Attempts to connect to the Temperature Pressure Humidity Sensor over SPI protocol.
	Returns:
		int: 0 means OK -1 means there was an error
	"""
	global bme280
	try:
	    bme280 = adafruit_bme280.Adafruit_BME280_SPI(spi, bme_cs)
	    return 0
	except:
            print("PTH Initialization Error")
            return -1

def temp():
	"""Gather temperature reading from BME280

	Returns:
		int: Temperature reading or -1 for error
	"""
	try:
	    return bme280.temperature
	except:
            print("PTH Temp Error")
            return -1

def humidity():
	"""Gather humidity reading from BME280

	Returns:
		int: Humidity reading or -1 for error
	"""	
	try:
	    return bme280.humidity
	except:
            print("PTH Hum Error")
            return -1

def pressure():
	"""Gather pressure reading from BME280

	Returns:
		int: Pressure reading or -1 for error
	"""
	try:
	    return bme280.pressure
	except:
            print("PTH Pres Error")
            return -1


def data():
	"""Gather readings from all three BME280 Sensors

	Returns:
		int array: [temp,humidity,pressure] ; -1 for any error values
	"""
	try:
	    return [temp(),humidity(),pressure()]
	except:
            return [-1,-1,-1]
