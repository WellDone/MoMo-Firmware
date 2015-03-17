#!/bin/bash

die() { echo "$@" 1>&2; exit 1; }
set -e

echo "MOMO_DEV: $MOMO_DEV"

apt-get update
apt-get install -y python python-setuptools python-dev python-pip
if [ -n "$MOMO_DEV" ]; then
	apt-get install -y libc6:i386 lib32stdc++6
fi
apt-get install wget

easy_install -U pip

pip install pyserial
pip install --allow-external intelhex --allow-unverified intelhex pymomo

pip install http://sourceforge.net/projects/scons/files/latest/download --egg | tee -a $HOME/scons-install.log
PYTHONPATH=`cat $HOME/scons-install.log | grep ' library modules ' | awk '{print $6}'`
echo "PYTHONPATH=$PYTHONPATH" >> $HOME/.profile

if [ -n "$TRAVIS" ]; then
	export MOMOPATH=`pwd`
	export DOWNLOADCACHE="~/.cached_downloads"
else # VAGRANT
	export MOMOPATH="/vagrant"
	export HOME="/home/vagrant"
	export DOWNLOADCACHE="/vagrant/.cached_downloads"

	echo "Adding user 'vagrant' to the group 'dialout' so it can access USB devices..."
	usermod vagrant -a -G dialout
	echo "DONE!"
fi
echo "export MOMOPATH=$MOMOPATH" >> $HOME/.profile
mkdir -p $DOWNLOADCACHE

download_file () {
	NAME=$1
	URL=$2
	FILE=`basename $URL`
	EXPECTEDSHA=$3
  if [ -e $DOWNLOADCACHE/$FILE ]; then
		echo "Found cached $NAME!"
		cp $DOWNLOADCACHE/$FILE .
	else
		echo "Downloading $NAME"
		wget $URL
		SHA=`openssl sha256 ./$FILE`
		if [ "$SHA" = "SHA256(./$FILE)= $EXPECTEDSHA" ]; then
			cp ./$FILE $DOWNLOADCACHE
		else
			#rm -f ./$FILE
			die "Invalid downloaded file hash, exiting!"
		fi
	fi
}

if [ -n "$MOMO_DEV" ]; then

	XC8VERSION=v1.34
	XC8INSTALLER=xc8-$XC8VERSION-full-install-linux-installer.run
	download_file "XC8 Installer"	http://ww1.microchip.com/downloads/en/DeviceDoc/$XC8INSTALLER ab0e36db27919fee2f41f95f345920e50fa3f32ba64f0accd2e041715ebdb231
	echo "Installing xc8 compiler..."
	chmod +x ./$XC8INSTALLER
	./$XC8INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc8/$XC8VERSION"
	CODE=$?
	echo "export PATH=\"\$PATH:/opt/microchip/xc8/$XC8VERSION/bin\"" >> $HOME/.profile
	rm -f ./$XC8INSTALLER $XC8INSTALLER.tar
	if [ $CODE -ne 0 ]; then
		die "Failed to install xc8, exiting!"
	fi
	echo "DONE!"

	XC16VERSION=v1.24
	XC16INSTALLER=xc16-$XC16VERSION-full-install-linux-installer.run
	download_file "XC16 Installer" http://ww1.microchip.com/downloads/en/DeviceDoc/$XC16INSTALLER 22fd2d5cb7042e8d9aa7aef8c2e5a8c7a76bf3c55e0b5ff2dbd23450e227e023
	echo "Installing xc16 compiler..."
	chmod +x ./$XC16INSTALLER
	./$XC16INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc16/$XC16VERSION"
	CODE=$?
	echo "export PATH=\"\$PATH:/opt/microchip/xc16/$XC16VERSION/bin\"" >> $HOME/.profile
	rm -f ./$XC16INSTALLER ./$XC16INSTALLER.tar
	if [ $CODE -ne 0 ]; then
		die "Failed to install xc16, exiting!"
	fi
	echo "DONE!"

	cd $HOME
	$MOMOPATH/tools/automation/install_patched_gpsim.sh
fi

echo "DONE!"

exit 0