## Programming Your MoMo
This section documents the process of getting the MoMo build environment set up and ready for hardware programming and testing.  The build system and support tools are cross-platform on Mac OS X, linux and Windows but are only tested extensively on Mac OS X and linux.  Bugfixes and patches are always gratefully accepted.  Windows users will need to install some supporting programs that are installed by default on Mac OS X and Linux, so read the [Windows Installation](windows-installation) first.

###Step 1 - Clone the Repository
Make sure git is installed on your computer and then clone our repository and checkout the dev branch
```shell
git clone https://github.com/WellDone/MoMo-Firmware.git
git checkout dev
```

###Step 2 - Run Vagrant or Develop Locally
There are two options here.  You can either:

* Install [VirtualBox](https://www.virtualbox.org/) and [Vagrant](http://www.vagrantup.com/downloads.html) and then run `vagrant up` from the repository directory.  This will download a VM (note: the initial download is large) and provision it with all of the development requirements.  Subsequent steps below can be run through `vagrant ssh` which creates a virtual shell connected to the running virtual machine.  At this point everything should be set up for you.
* OR Install the development dependencies locally.  There may be some idiosyncacies to work through, but you should be able to mostly follow the instructions [here](http://www.github.com/WellDone/MoMo-Firmware/wiki/Development-Dependencies).

### Looking Around
```shell
> ls
config  doc  momo_modules  pcb  README.md  tools
```

A brief overview of what is what:
* config - reference information about pcb layouts and build constants.  Templates for module skeleton code is stored here as well
* doc - assorted datasheets and reference information.  Not terribly well organized currently
* momo_modules - all module firmware is stored under these directories.  momo_modules/shared contains shared library code.  Other directories correspond to a single firmware image each.
* pcb - all circuit board
* tools - all of the support infrastructure is stored in the tools directory.  You should add tools/bin to your PATH for easy access to the support tools.  Most of the build system is implemented as a python package pymomo which lives in tools

### Step 3 - Required Configuration
This is only required if you want to use pcbtool to automate processing of circuit board designs.  You must edit the file config/settings.local.json to contain the path to your local copy of EAGLE.

```shell
> cat config/settings.local.json 
{
	"external_tools":
	{
		"eagle": "<INSERT PATH TO EAGLE HERE>",
		"gpsim": "/opt/pic/bin/gpsim"
	}
}
```

In order to use the unit testing infrastructure for pic12 code, you must also configure the gpsim path but this is not yet supported since we require a modified version of gpsim (to fix many bugs in the main codebase that we hit).  Our internal gpsim version will be released soon and then unit testing will be available for everyone.

### Checking that Everything Works
Assuming that you added tools/bin to your PATH variable, you can use the modtool program to talk to a connected MoMo unit.  Plug in your MoMo FSU into a USB port and plug the other side into a MoMo unit.  Power the MoMo unit and you should see two green lights illuminate.  Usually modtool will be able to guess which port your MoMo is plugged into, if not you will need to know the device or com port that corresponds to the MoMo FSU.  

On linux it is probably:
```shell
/dev/ttyUSB0
```

On Mac OS X is is probably:
```shell
/dev/tty.usbserial.SOMETHING
```

On Windows is is probably COM2 or COM3 depending on how many other serial ports you have.  If you are using cygwin (which we don't recommend), then the serial port will show up probably as something like:
```shell
/dev/ttyS2
```

**Note:**  On OS X Mavericks, apple broke FTDI USB-Serial adapter support so you need to implement the workaround described here to install a working driver: http://forum.arduino.cc/index.php?topic=198539.0

If you need a driver for the MoMo FSU, it uses the FTDI 230X chip which has drivers here:
http://www.ftdichip.com/FTDrivers.htm

In what follows, _PORT_ will refer to the device or COM port as listed above that corresponds to your computer.  Try to access the MoMo and list all of its attached modules to make sure everything is working:
```shell
modtool list
0: pv_mod
1: Whatever
2: Something else
```

If the command finishes without error, you are completely set up.  If it throws an exception then it was not able to guess your COM port correctly and you'll have to specify it explicitly, i.e.:
```shell
modtool list -p PORT
0: pv_mod
1: Whatever
2: Something else
```

### Step 4 - Read up on how to use all of the tools
There is documentation for all of the support tools that help you use MoMo.  Read about them:
* [[modtool|modtool]]