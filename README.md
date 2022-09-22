# BESI-C Sensors
Environmental sensors reader, audio data analyzer, and testing data generators


## Build Audio Package
Install build dependencies

	sudo apt-get -y install devscripts debhelper python3 python3-numpy python3-matplotlib python3-scipy python3-pyaudio python3-tqdm jackd libjack-jackd2-dev portaudio19-dev

Then enter package directory and build package

	cd audio-py
	debuild -uc -us


## Build Environmental Sensor Package
Install build dependencies

	sudo apt-get -y install devscripts debhelper libbesic-dev libpigpio-dev

Then enter package directory and build package

	cd envsense
	debuild -uc -us


## Build Null Sensor Package
Install build dependencies

	sudo apt-get -y install devscripts debhelper libbesic-dev

Then enter package directory and build package

	cd nullsense
	debuild -uc -us


## Build Dummy CSV Package
Install build dependencies

	sudo apt-get -y install devscripts debhelper libbesic-dev

Then enter package directory and build package

	cd dummy-csvs
	debuild -uc -us
