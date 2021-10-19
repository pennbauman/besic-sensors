#!/usr/bin/python3
import os

settings = {
    "STORAGE_PATH": "/var/besic/data/sensors.csv",
    "SOUND_PATH": "/var/besic/data/audio.csv",
}

try:
    file = open("./config").read().strip().split('\n')

    for line in file:
        line = line.strip()
        if (len(line) == 0):
            continue
        if (line[0] == '#'):
            continue
        try:
            line = line.split('=')
            key = line[0].strip()
            value = line[1].strip().strip('"')
            settings[key] = value
        except Exception as e:
            print(e)
            continue
except:
    pass

try:
    settings['DEPLOYMENT'] = os.environ['DEPLOYMENT_NAME']
except:
    pass

try:
    settings['ID'] = os.environ['RELAY_ID']
except:
    pass


def get():
    if (len(settings) > 0):
        return settings
    raise ValueError("no keys found")
