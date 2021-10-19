#!/usr/bin/python3
import lib.BME as BME
import lib.LUX as LUX
import lib.config as config
from lib.Heartbeats import sendHeartBeat
import time,os,sys,threading

print("Started Sensor Program")

BME.init()
LUX.init()

vars = config.get()
try:
    deployment = vars['DEPLOYMENT']
    relay = vars['ID']
    filepath = vars['STORAGE_PATH']
except:
    print("Config Error")
    exit(1)

print("DEPLOYMENT: " + deployment)
print("Relay ID: " + relay)
print("STORAGE_PATH: " + filepath)

# Performs Audio Data Collection and Analysis
def CollectSound():
    import lib.Sound as Sound
    while(1):
        Sound.GetData()

if (len(sys.argv) == 1) or (sys.argv != "noaudio"):
    # Runs CollectSound in a new thread
    SoundThread = threading.Thread(target=CollectSound, args=(), daemon=True)
    SoundThread.start()

file_headers = "timestamp,light,temp,pres,hum\n"

#Main Loop - Collects data from all sensors and sends heartbeats
while True:
    timestamp = time.time()

    temp, humidity, pressure = BME.data()
    light = LUX.lux()
    #print('Temp={0:0.1f}*C Humidity={1:0.1f}% Pressure={2:0.1f} Light={3:0.1f}'.format(temp, humidity, pressure, light))
    
    sendHeartBeat(relay,deployment,light,temp,pressure,humidity)
    
    temperature = '%.2f'%(temp)
    humidity = '%.2f'%(humidity)
    pressure = '%.2f'%(pressure)
    light = '%.2f'%(light)

    try:
        with open(filepath,'a') as file:
            file.write("{},{},{},{},{}\n".format(timestamp,light,temperature,pressure,humidity))
    except FileNotFoundError as e:
        with open(filepath,'w') as file:
            file.write(file_headers)
    except Exception as e:
        print(e)

    time.sleep(1)
