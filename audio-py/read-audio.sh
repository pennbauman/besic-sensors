#!/bin/bash
# Run audio data collection

# Check configuration file
source "/etc/besic/audio.conf"
if [ -z $AUDIO_PATH ]; then
	export AUDIO_PATH="/var/besic/data/audio.csv"
else
	export AUDIO_PATH="$AUDIO_PATH"
fi

python3 /usr/share/besic/audio-py/AudioRead.py
