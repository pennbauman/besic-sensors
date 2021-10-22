#!/usr/bin/python3
import math, wave, time

from threading import Thread

try:
    import config as config
    import AudioFeatures as ShortTermFeatures
    #from pyAudioAnalysis import ShortTermFeatures
except:
    import lib.config as config
    import lib.AudioFeatures as ShortTermFeatures
    #from lib.pyAudioAnalysis import ShortTermFeatures

from ctypes import *
from contextlib import contextmanager
import pyaudio
import numpy as np

_vars = config.get()
SOUND_PATH = _vars["SOUND_PATH"]

CHUNKSIZE = 512   # fixed chunk size
RATE = 8000      # 16kHz Sampling rate
SAMPLE_DUR = 0.5 # 250ms sampling with 50% overlap

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


# -- This code snippet was used to determine ID of I2S Microphone -- #

# Get input device ID numbers
def Identify_Devices(init=False):
    info = p.get_host_api_info_by_index(0)
    numdevices = info.get('deviceCount')
    for i in range(0, numdevices):
        if (p.get_device_info_by_host_api_device_index(0, i).get('maxInputChannels')) > 0:
            if init:
                return i
            
            print ("Input Device id ", i, " - ", p.get_device_info_by_host_api_device_index(0, i).get('name'))

# -- Init *MUST* be called before any of the other functions -- #
#it must also be called if Terminate() is called to resume audio stream
stream = None
first_half = []#np.empty(1)
second_half = []#np.empty(1)
combinedData = []#np.empty(1)
counter = 0


def StoreAnalysis():
    return

def AnalyzeData(data):
    F, f_names = ShortTermFeatures.feature_extraction(data, RATE, SAMPLE_DUR*RATE, (SAMPLE_DUR/2)*RATE,deltas=False)
    # F is the data, so save that to the CSV file for audio data
    #for i in range(len(F)):
    #    print(f_names[i],F[i])
    return F,f_names


# Only to be used in case that you want to store audio samples
def WriteData(data):
    t = time.time() # use to generate timestamp for file name
    filename = "RawAudio.wav" # <- file naming convention
    wf = wave.open(filename,'wb') #Open wave file in write bytes mode
    wf.setnchannels(1)     #Mono channel audio
    wf.setsampwidth(4)    #Using 32-bit audio samples - 4 bytes
    wf.setframerate(RATE) #Set the sampling freq of audio
    wf.writeframes(b''.join(data))
    wf.close()
    return

def CombineData(ts,first_half,second_half):
    global combinedData
    combinedData = []

    #print(type(first_half))
    #print(type(second_half))
    #for i in first_half:
    #    print(i)

    if counter%2 == 1:
        combinedData.append(first_half)
        combinedData.append(second_half)
        #combinedData = np.concatenate(second_half,first_half).ravel().T
    else:
        combinedData.append(second_half)
        combinedData.append(first_half)
        #combinedData = np.concatenate(first_half,second_half).ravel().T
    
    cD = np.concatenate(combinedData)
    F,fn = AnalyzeData(cD)

    #Write to CSV File:
    try:
        with open(SOUND_PATH,'a+') as file:
                file.write(str(ts))
                for feature in F:
                    file.write(","+str(feature[0]))
                file.write("\n")
    except Exception as e:
        print(e)


    # -- NO LONGER NEEDED -- #
    # Write to .wav file on another thread
    # WriteData(combinedData)

def DataThread(ts,fh,sh):
    #Copy data
    f_h = np.copy(fh)
    s_h = np.copy(sh)
    #Do everything in a new thread
    thread = Thread(target=CombineData, args=(ts,f_h,s_h,))
    thread.start()
    #CombineData(f_h,s_h)

# -- Gathers data from the micrphone and calls WriteData then AnalyzeData() -- #


def GetData(test_mode:bool = False, store_data:bool = True):
    global first_half, second_half, counter
    
    timestamp = time.time()

    npd = []#np.empty(1,)
    #d = []
    
    if test_mode:
        t = time.time()
    
    # -- Take 250ms of sound and put it in either first/second half -- #
    for _ in range(0, int(math.ceil(RATE/CHUNKSIZE * (SAMPLE_DUR/2)))):
        data = stream.read(CHUNKSIZE,exception_on_overflow=False)
        numpydata = np.frombuffer(data, dtype=np.uint32)
        npd.append(numpydata)

    if test_mode:
        t2 = time.time()

    if store_data:
        if counter % 2 == 0:
            first_half = np.concatenate(npd).ravel()#np.array(npd)
        else:
            second_half = np.concatenate(npd).ravel()#np.array(npd)

        if counter > 0:
            DataThread(timestamp,first_half,second_half)
    
    if test_mode:
        t3 = time.time()

        rectime = t2-t
        proctime = t3-t2
        alltime = t3-t
        if counter>0:
            print("Recording Time:    ",rectime)
            print("Processing Time:   ",proctime)
            print("Total Time Elapsed:",alltime)

    counter += 1

    if test_mode:
        return npd

def StopAudoStream():
    stream.stop_stream()
    stream.close()

if __name__ == "__main__": 
    Identify_Devices()

else:
    devId = Identify_Devices(init=True)
    if devId is None:
        exit(1)
    stream = p.open(format=pyaudio.paInt32, channels=1, rate=RATE,input=True, frames_per_buffer=CHUNKSIZE,input_device_index = devId)
