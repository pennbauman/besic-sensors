#!/bin/bash
# BESI-C Relay Sensor Setup
#   https://github.com/pennbauman/besic-relay
#   Penn Bauman <pcb8gb@virginia.edu>

DIR="/var/besic"
loc="$(dirname $BASH_SOURCE)"
LOG="/var/log/besic/sensors_init.log"

# Edit system configs
sed -i 's/#dtparam=spi=on/dtparam=spi=on/' /boot/config.txt
sed -i 's/#dtparam=i2s=on/dtparam=i2s=on/' /boot/config.txt
sed -i 's/#dtparam=i2c_arm=on/dtparam=i2c_arm=on/' /boot/config.txt

echo "snd-bcm2835" > /etc/modules
echo "snd-i2smic-rpi" >> /etc/modules
echo "i2c-dev" >> /etc/modules
echo "options snd-i2smic-rpi rpi_platform_generation=0" > /etc/modprobe.d/snd-i2smic-rpi.conf

modprobe snd-i2s_rpi rpi_platform_generation=0

echo "[$(date --rfc-3339=seconds)]: Interfaces enabled" >> $LOG


# Install system packages
SENSOR_PKG="i2c-tools"
PYTHON_PKG="python3-pip python3-setuptools python3-matplotlib libatlas-base-dev"
AUDIO_PKG="jack libjack-jackd2-dev portaudio19-dev dkms raspberrypi-kernel-headers"

apt update
apt -y install git $SENSOR_PKG $PYTHON_PKG $AUDIO_PKG 2>> $LOG

echo "[$(date --rfc-3339=seconds)]: Packages installed" >> $LOG

# Setup audio driver
DRIVER_URL="https://github.com/adafruit/Raspberry-Pi-Installer-Scripts.git"
git clone $DRIVER_URL $DIR/adafruit
cp -R $DIR/adafruit/i2s_mic_module /usr/src/snd-i2s_rpi-0.1.0
dkms add -m snd-i2s_rpi -v 0.1.0 2>> $LOG
dkms build -m snd-i2s_rpi -v 0.1.0 2>> $LOG
dkms install -m snd-i2s_rpi -v 0.1.0 2>> $LOG

#DRIVER_URL="https://github.com/opencardev/snd-i2s_rpi/releases/download/v0.0.2/snd-i2s-rpi-dkms_0.0.2_all.deb"
#DRIVER_URL=$(curl -s https://api.github.com/repos/opencardev/snd-i2s_rpi/releases/latest | grep "browser_download_url.*deb" | cut -d : -f 2,3 | cut -d \" -f 2)
#file=$(echo $DRIVER_URL | sed 's/^.*\///')
#curl -L -o $file $DRIVER_URL
#dpkg -i $file
#rm -f $file

echo "[$(date --rfc-3339=seconds)]: Audio driver installed" >> $LOG

# Install python packages
pip3 install "board==1.0" 2>> $LOG
pip3 install -r $loc/requirements.txt 2>> $LOG

# Setup systemd service
if [[ $loc == "$DIR/sensors" ]]; then
	cp $loc/besic.sensors.service /etc/systemd/system
	systemctl enable besic.sensors.service 2>> $LOG
	systemctl start besic.sensors.service 2>> $LOG

	echo "[$(date --rfc-3339=seconds)]: Systemd service setup" >> $LOG
fi

rm $DIR/init.sh

apt -y upgrade 2>> $LOG

rm $(dirname $LOG)/sensors.log

echo "[$(date --rfc-3339=seconds)]: Setup complete" >> $LOG

reboot
