#!/usr/bin/python3
# BESI-C Audio Analyzer
#   https://github.com/besi-c/besic-sensors
#   Yudel Martinez <yam3nv@virginia.edu>
#   Penn Bauman <pcb8gb@virginia.edu>
import time
import os
import sys
import besic
import lib.Mic as Mic


# Run test based on arguments
if (len(sys.argv) > 1) and (sys.argv[1] == "test"):
    print("Beginning Test")
    # Take two audio readings
    t1 = time.time()
    data1 = Mic.RawData()
    t2 = time.time()
    data2 = Mic.RawData()
    t3 = time.time()

    # Analyze data
    features = Mic.ProcessData(t1, data1, data2, 0, None)
    t4 = time.time()

    # Calculate times
    rectime1 = t2 - t1
    rectime2 = t3 - t2
    proctime = t4 - t3
    alltime = t4 - t1
    print("Recording Time 1:  ", rectime1)
    print("Recording Time 2:  ", rectime2)
    print("Processing Time:   ", proctime)
    print("Total Time Elapsed:", alltime)
    # Print features
    print("Features: " + str(features))
    sys.exit()


# Determine data file path
AUDIO_PATH = os.path.join(besic.data_dir(), "audio.csv")
print("Starting Audio Collection")
print("> Writing to " + AUDIO_PATH)


counter = 0
first_half = None
second_half = None

# Read data
while True:
    timestamp = time.time()
    raw_data = Mic.RawData()

    # Store data in alternating order
    if counter % 2 == 0:
        first_half = raw_data
    else:
        second_half = raw_data

    # Analyze data if multiple readings made
    if counter > 0:
        Mic.DataThread(timestamp, first_half, second_half, counter, AUDIO_PATH)

    counter += 1
