echo "Installing GPSIM..."
set -e

GPSIM_VERSION="0.28.0"

GPSIM_PATH=`pwd`/gpsim
mkdir $GPSIM_PATH
wget -O - https://github.com/welldone/gpsim/releases/download/$GPSIM_VERSION/gpsim.tar.gz | tar xz -C $GPSIM_PATH

echo "export PATH=\$PATH:$GPSIM_PATH/bin" >> $HOME/.profile
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$GPSIM_PATH/lib" >> $HOME/.profile

echo "Done!"