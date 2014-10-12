#!/bin/bash

die() { echo "$@" 1>&2; exit 1; }

apt-get update
apt-get install -y python python-setuptools python-dev libc6:i386 lib32stdc++6 gpsim

easy_install pip
easy_install intelhex # This doesn't work with PIP for some reason
pip install cmdln ZODB3 colorama pyparsing BeautifulSoup4 Cheetah pyserial pytest decorator pycparser

if [ -n "$TRAVIS" ]; then
	MOMOROOT=`pwd`
	DOWNLOADCACHE="~/.cached_downloads"
else # VAGRANT
	MOMOROOT="/vagrant"
	HOME="/home/vagrant"
	DOWNLOADCACHE="/vagrant/.cached_downloads"

	echo "Adding user 'vagrant' to the group 'dialout' so it can access USB devices..."
	usermod vagrant -a -G dialout
	echo "DONE!"
fi
echo "MOMOROOT=$MOMOROOT" >> $HOME/.profile
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

SCONSVERSION=2.3.3
download_file "SCONS Installer" http://downloads.sourceforge.net/project/scons/scons/$SCONSVERSION/scons-$SCONSVERSION.tar.gz 63a74e1012ad9ad54950eafc69604fd9246e1a405c18acf936caa433c97cc0e2
tar -xvzf scons-$SCONSVERSION.tar.gz
cd scons-$SCONSVERSION
python setup.py install
echo "PYTHONPATH=\".:/usr/local/lib/scons-$SCONSVERSION\"" >> $HOME/.profile
cd ..

XC8INSTALLER='xc8-v1.30-linux.run'
download_file "XC8 Installer"	http://ww1.microchip.com/downloads/en/DeviceDoc/$XC8INSTALLER.tar 1ab6186eda45f774cfad1e2a0c7a4cbe449991b363634545c3eb005667925722
tar -xvf $XC8INSTALLER.tar
echo "Installing xc8 compiler..."
./$XC8INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc8/v1.30"
CODE=$?
echo "PATH=\"\$PATH:/opt/microchip/xc8/v1.30/bin\"" >> $HOME/.profile
rm -f ./$XC8INSTALLER $XC8INSTALLER.tar
if [ $CODE -ne 0 ]; then
	die "Failed to install xc8, exiting!"
fi
echo "DONE!"

XC16INSTALLER=xc16-v1.21-linux-installer.run
download_file "XC16 Installer" http://ww1.microchip.com/downloads/en/DeviceDoc/$XC16INSTALLER.tar 80a9fcc6e9e8b051266e06c1eff0ca078ebbc791a7d248dedd65f34a76d7735c
tar -xvf $XC16INSTALLER.tar
echo "Installing xc16 compiler..."
./$XC16INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc16/v1.21"
CODE=$?
echo "PATH=\"\$PATH:/opt/microchip/xc16/v1.21/bin\"" >> $HOME/.profile
rm -f ./$XC16INSTALLER ./$XC16INSTALLER.tar
if [ $CODE -ne 0 ]; then
	die "Failed to install xc16, exiting!"
fi
echo "DONE!"

echo "Adding MoMo tool bin ($MOMOROOT/tools/bin) to the path..."
echo "PATH=\"\$PATH:$MOMOROOT/tools/bin\"" >> $HOME/.profile
echo "DONE!"

exit 0