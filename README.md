# BESI-C Sensors
Code for BESI-C environmental sensors and audio feature extractor


## Build Audio Package
Install build dependencies

	sudo apt-get -y install devscripts debhelper python3 python3-numpy python3-matplotlib python3-scipy python3-pyaudio python3-tqdm jackd libjack-jackd2-dev portaudio19-dev

Then enter package directory and build package

	cd audio-py
	debuild -uc -us


## Build Sensor Package
On a raspberry pi, install build dependencies

	sudo apt-get -y install devscripts debhelper libbesic2 libpigpio1 libbesic2-dev libpigpio-dev

Then enter package directory and build package

	cd envsense
	debuild -uc -us
