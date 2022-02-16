#!/bin/bash
# Run environmental data

# Get device configuration
source besic-url-conf
source besic-deploy-conf
source besic-dev-conf
if [ $? -ne 0 ]; then
	echo "[$(date --rfc-3339=seconds)] Device configuration failed" >> $LOG
	exit 1
fi

# Check configuration file
source "/etc/besic/environ.conf"
if [ -z $ENVIRON_PATH ]; then
	export ENVIRON_PATH="/var/besic/data/data.csv"
else
	export ENVIRON_PATH="$ENVIRON_PATH"
fi

python3 /usr/share/besic/audio-py/EnvironRead.py
