# Contributing

Development can be done in a virtual machine using [Vagrant](http://www.vagrantup.com) (recommended, see [Developing with Vagrant](#contributing-developing-with-vagrant)) or locally (See [Local Development Dependencies](#contributing-local-development-dependencies))

## Cloning the repository

To develop for MoMo, you need to download the source code and tool chain from the repository.  This requires [git](http://git-scm.com/).

First, download the git command line tool for your platform.  If you're on Mac OSX I recommend GitHub's wrapper [hub](http://github.com/github/hub).

Next, clone the repository by running `git clone https://github.com/WellDone/MoMo-Firmware.git`.  Run `cd MoMo-Firmware && ls` and you'll see that you now have a local copy of the latest and greatest MoMo source code!

## Developing with Vagrant

This is the recommended development method as it ensures compliance with the automated build system.  It's also very easy to set up and tear down, but does have drawbacks because you have to run a virtual machine and download some large files (~500MB) when first setting up the environment.  If this scares you, install the [Local Development Dependencies](#contributing-local-development-dependencies) before proceeding.

To develop with Vagrant:

- Install [Vagrant](http://www.vagrantup.com)
- Install [VirtualBox](https://www.virtualbox.org/)
- Run `vagrant up` from the source directory.  This will create a new virtual machine and install all of the development dependencies on it.
- Finally, run `vagrant ssh` to work in the newly-created VM and you'll see that the source directory is accessible at /welldone, so `cd /welldone` and you're ready to rock and roll.

## Local Development Dependencies

If you choose to manually install all development dependencies (rather than using Vagrant, which is recommended) the following steps should get you up and running.

For reference, the automated build system uses the script at `tools/automation/provision.sh` to install all dependencies.

### XC8 and XC16 compilers

The following scripts are not guaranteed to run everywhere, but should give you an idea of how to get the installers and run them.  If you can't tell what these scripts will do by looking at them, proceed with caution.

*XC8*
```shell
XC8INSTALLER='xc8-v1.30-linux.run'

# Download XC8
wget	http://ww1.microchip.com/downloads/en/DeviceDoc/$XC8INSTALLER.tar
tar -xvf $XC8INSTALLER.tar

# Install XC8
./$XC8INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc8/v1.30"
CODE=$?

# Add the new compiler to your PATH variable
echo "PATH=\"\$PATH:/opt/microchip/xc8/v1.30/bin\"" >> $HOME/.profile
source $HOME/.profile

# Clean up installation files
rm -f ./$XC8INSTALLER $XC8INSTALLER.tar
```

*XC16*
```shell
XC16INSTALLER=xc16-v1.21-linux-installer.run

# Download XC16
wget http://ww1.microchip.com/downloads/en/DeviceDoc/$XC16INSTALLER.tar
tar -xvf $XC16INSTALLER.tar


# Install XC16
./$XC16INSTALLER --mode unattended --netservername "" --prefix "/opt/microchip/xc16/v1.21"

# Add the new compiler to your PATH variable
echo "PATH=\"\$PATH:/opt/microchip/xc16/v1.21/bin\"" >> $HOME/.profile
source $HOME/.profile

# Clean up installation files
rm -f ./$XC16INSTALLER ./$XC16INSTALLER.tar
```

### Required Python Libraries

The MoMo build system and associated tools are written in python.  They have been tested with Python 2.7 and may or may not work with either older or new versions.  It also depends on the following external python packages:

- cmdln
- ZODB3
- colorama
- pyparsing
- BeautifulSoup4
- Cheetah
- pyserial
- pytest
- decorator
- pycparser
- intelhex
- scons

All but intelhex and scons are easily installable via [PIP](https://pypi.python.org/pypi/pip), so something like the following sequence should work:
```shell
# Install PIP if you don't have it already
wget https://bootstrap.pypa.io/get-pip.py
sudo python get-pip.py

# Install all the required python libraries
sudo pip install cmdln ZODB3 colorama pyparsing BeautifulSoup4 Cheetah pyserial pytest decorator pycparser
sudo easy_install intelhex # This doesn't work with PIP for some reason, so for now we use setuptools
```

*Note* As shown below, the scons python packages can be downloaded from [scons.org](http://www.scons.org/download.php) and installed from there.  Note that if you get an error like `ImportError: No module named SCons.Script`, make sure the path to the SCONS installation is in the PYTHONPATH environment variable.

```shell
SCONSVERSION=2.3.3

# Download SCONS
wget http://downloads.sourceforge.net/project/scons/scons/$SCONSVERSION/scons-$SCONSVERSION.tar.gz
tar -xvzf scons-$SCONSVERSION.tar.gz

# Install SCONS
cd scons-$SCONSVERSION
python setup.py install

# Add SCONS to your PYTHONPATH (IMPORTANT)
echo "PYTHONPATH=\".:/usr/local/lib/scons-$SCONSVERSION\"" >> $HOME/.profile
```

Note that on Mac OS X you will have to use sudo to install the packages as root.  On linux you probably will as well depending on your python installation. If you want to work with the circuit board layouts, you must install [CADSoft Eagle](https://www.cadsoftusa.com/download-eagle) version at least 6.5.0.

### Simulator Support

Unit tests are run using Microchip's SIM30 simulator (installed with XC16) for PIC24 modules, and using the open-source [GPSIM](http://gpsim.sourceforge.net).  You can often install GPSIM using your Operating System's built-in package manager (on Ubuntu just run `sudo apt-get update && sudo apt-get install gpsim`), otherwise follow the instructions on their website.  When you've finished, make sure `gpsim` is in your PATH so the build tools can find it by running `which gpsim` and making sure it finds something.

#### Patching GPSIM (since we need a customized version)
You need to download and patch gpsim from svn.  This section assumes that you are checking out gpsim into the parent directory containing MoMo-Firmware.  If this is not the case you will need to modify the path to gpsim_16lf1847_support.patch based on your system.

```shell
$ mkdir gpsim
$ cd gpsim
$ svn checkout svn://svn.code.sf.net/p/gpsim/code/trunk -r 2281 .
<Lots of text>
$ patch -p0 -i ../MoMo-Firmware/support/gpsim_16lf1847_support.patch
patching file src/14bit-hexdecode.cc
patching file src/14bit-instructions.h
patching file src/Makefile.am
patching file src/makefile.mingw
patching file src/p16f1847.cc
patching file src/p16f1847.h
patching file src/p1xf1xxx.cc
patching file src/p1xf1xxx.h
patching file src/pic-instructions.cc
patching file src/pic-processor.cc
patching file src/pic-processor.h
```

Now you need to follow the [instructions](http://gpsim.sourceforge.net/gpsim_svn.html) to build and install gpsim.  For reference, the current required commands are:

```shell
$ libtoolize --force
$ aclocal
$ autoheader
$ automake --force-missing --add-missing
$ autoconf
$ ./configure
$ make
$ sudo make install
```

Make sure you install gpsim into your $PATH correctly and you should be able to build pic12 unit tests.  As a test, navigate to the pic12_executive directory
and make sure you can build the tests:
```shell
$ cd MoMo-Firmware/momo_modules/pic12_executive
$ momo build test -c
$ momo build test
```

You need to have the pro mode compiler currently to build the pic12_executive. 