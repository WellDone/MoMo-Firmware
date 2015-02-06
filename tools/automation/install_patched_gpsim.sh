echo "Patching GPSIM..."
set -e

apt-get install -y g++ subversion automake make libglib2.0-dev pkg-config libpopt-dev libtool flex bison

rm -rf gputils
mkdir gputils
cd gputils

svn checkout svn://svn.code.sf.net/p/gputils/code/trunk/gputils -r 1128 .

./configure
make
make install

cd ..

rm -rf gpsim
mkdir gpsim
cd gpsim

svn checkout svn://svn.code.sf.net/p/gpsim/code/trunk -r 2299 .
patch -p0 -i $MOMOPATH/support/gpsim_16lf1847_support.patch

libtoolize --force
aclocal
autoheader
automake --force-missing --add-missing
autoconf
./configure --disable-gui
make
make install