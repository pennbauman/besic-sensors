#!/usr/bin/python3
import lib.BME as BME
import lib.LUX as LUX
import lib.config as config
import time, os, sys, threading

BME.init()
LUX.init()

def CollectSound():
    import lib.Sound as Sound
    while(1):
       data_sample =  Sound.GetData(test_mode=True, store_data=False)
       print("Sound Data",data_sample,"\n")
       break

SoundThread = threading.Thread(target=CollectSound, args=(), daemon=True)
SoundThread.start()

while True:
    temp, humidity, pressure = BME.data()
    light = LUX.lux()
    print('Temp={0:0.1f}*C Humidity={1:0.1f}% Pressure={2:0.1f} Light={3:0.1f}'.format(temp, humidity, pressure, light))
    time.sleep(1)
