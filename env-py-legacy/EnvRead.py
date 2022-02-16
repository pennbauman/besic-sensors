#!/usr/bin/python3
import time
import os
import sys
import lib.BME as BME
import lib.LUX as LUX
from lib.Heartbeat import sendApiHeartbeat

FILE_HEADERS = "timestamp,light,temp,pres,hum\n"


BME.init()
LUX.init()


# Run test based on arguments
if (len(sys.argv) > 1) and (sys.argv[1] == "test"):
    tmp, hum, prs = BME.data()
    lux = LUX.lux()

    print("Light = {0.1f}".format(tmp))
    print("Temperature = {0.1f}°C".format(tmp))
    print("Pressure = {0.1f} Pa".format(tmp))
    print("Humidity = {0.1f}%".format(tmp))
    sys.exit()


# Determine data file path
ENVIRON_PATH = os.getenv("ENVIRON_PATH")
if not ENVIRON_PATH:
    ENVIRON_PATH = "/var/besic/data/data.csv"

print("Starting Environ Collection")
print("> Writing to " + ENVIRON_PATH)


# Read data
while True:
    timestamp = time.time()

    tmp, hum, prs = BME.data()
    lux = LUX.lux()

    # Send heartbeat
    sendApiHeartbeat(lux, tmp, prs, hum)

    # Write to file
    try:
        with open(ENVIRON_PATH, 'a') as file:
            file.write("{},{},{},{},{}\n".format(timestamp, lux, tmp, prs, hum))
    except FileNotFoundError as e:
        with open(ENVIRON_PATH, 'w') as file:
            file.write(FILE_HEADERS)
    except Exception as e:
        print(e)

    time.sleep(1)
