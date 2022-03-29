#!/usr/bin/python3
import sys
import math
import wave
import time
from threading import Thread
from ctypes import *
from contextlib import contextmanager
import pyaudio
import numpy as np

try:
    import AudioFeatures as ShortTermFeatures
except:
    import lib.AudioFeatures as ShortTermFeatures


# Constant settings
CHUNKSIZE = 512    # fixed chunk size
RAW_RATE = 44100   # 44100Hz sampling rate used by mic
DATA_RATE = 16000  # 16kHz sampling rate used for data analysis
SAMPLE_DUR = 0.5   # 250ms sampling with 50% overlap


# suppress PyAudio / Alsa error messages
ERROR_HANDLER_FUNC = CFUNCTYPE(None, c_char_p, c_int, c_char_p, c_int, c_char_p)
def error_ignorer(filename, line, function, err, fmt):
    pass
c_error_handler = ERROR_HANDLER_FUNC(error_ignorer)

@contextmanager
def noalsaerr():
    asound = cdll.LoadLibrary('libasound.so')
    asound.snd_lib_error_set_handler(c_error_handler)
    yield
    asound.snd_lib_error_set_handler(None)

# initialize pyaudio
with noalsaerr():
    p = pyaudio.PyAudio()



# Get input I2S device ID numbers
def Identify_Devices(init=False):
    info = p.get_host_api_info_by_index(0)
    numdevices = info.get('deviceCount')
    for i in range(0, numdevices):
        if (p.get_device_info_by_host_api_device_index(0, i).get('maxInputChannels')) > 0:
            if init:
                return i
            print ("Input Device id ", i, " - ", p.get_device_info_by_host_api_device_index(0, i).get('name'))


# -- Init *MUST* be called before any of the other functions -- #
# it must also be called if Terminate() is called to resume audio stream
stream = None
first_half = []
second_half = []
combinedData = []


# Get features from audio data
def AnalyzeData(data):
    return ShortTermFeatures.feature_extraction(data, DATA_RATE, SAMPLE_DUR*DATA_RATE, (SAMPLE_DUR/2)*DATA_RATE,deltas=False)



def ProcessData(ts, first_half, second_half, i, path):
    global combinedData
    combinedData = []
    # Combine data with switching order
    if i % 2 == 1:
        combinedData.append(first_half)
        combinedData.append(second_half)
    else:
        combinedData.append(second_half)
        combinedData.append(first_half)

    # Combine and analyze data
    cD = np.concatenate(combinedData)
    F, fn = AnalyzeData(cD)

    # Return data if not writing
    if path == None:
        return F
    # Write to CSV file
    try:
        with open(path, 'a+') as file:
            file.write(str(ts))
            for feature in F:
                file.write("," + str(feature[0]))
            file.write("\n")
    except Exception as e:
        print(e)


# Thread to combine and write data
def DataThread(ts, fh, sh, i, path):
    # Copy data
    f_h = np.copy(fh)
    s_h = np.copy(sh)
    # Do everything in a new thread
    thread = Thread(target=ProcessData, args=(ts, f_h, s_h, i, path))
    thread.start()


# Read raw audio data
def RawData():
    npd = []
    # Read data
    for _ in range(0, int(math.ceil(DATA_RATE/CHUNKSIZE * (SAMPLE_DUR/2)))):
        data = stream.read(CHUNKSIZE,exception_on_overflow=False)
        numpydata = np.frombuffer(data, dtype=np.uint32)
        npd.append(numpydata)
    time.sleep(0.4)
    # Convert data to numpy
    return np.concatenate(npd).ravel()


# Stop audio reading
def StopAudoStream():
    stream.stop_stream()
    stream.close()


# Setup mic devices
if __name__ == "__main__":
    Identify_Devices()
else:
    devId = Identify_Devices(init=True)
    if devId is None:
        print("No devices identified")
        sys.exit(1)
    stream = p.open(format=pyaudio.paInt32, channels=1, rate=RAW_RATE, input=True, frames_per_buffer=CHUNKSIZE, input_device_index = devId)
