##modtool - Working with MoMo hardware modules

modtool is a program that sends commands to MoMo hardware modules and allows you to test them, debug them and program them.
It is written in python and all of its functionality is built on top of the pymomo.commander library.  If you are comfortable
in Python and looking to script your MoMo module, you should jump directly to using pymomo.commander in Python scripts.  If
you are looking for a command line application to simplify common MoMo related tasks, modtool is right for you.  Modtool
runs equally well on Mac OS X, Linux and Windows provided that you have installed all of its library dependencies.

### Introduction
modtool can do the following things:
 * Reprogram any MoMo module with new firmware
 * Run premade hardware testing scripts on any MoMo module
 * List attached modules and interact with them
 * Configure any MoMo module
 * Debug MoMo modules
 * Dump MoMo sensor or system logs

### Using Modtool
modtool is a command line program that lives in tools/bin.  If you have added tools/bin to your $PATH, you can run modtool
by executing:

```shell
$ modtool help
```

All of the modtool commands are documented in the internal help system, which can be accessed by using:

```shell
$ modtool help subcommand
```

### Connecting Your Hardware
Using modtool requires that you connect your MoMo device to your computer using a bridge between your computer
and the MoMo bus (MIB).  This could be done using a MoMo Field Service Unit (FSU) that connects to the MoMo and
to the USB port of your computer, or it could be done over bluetooth.  In either case, your fsu will show up as a
COM port and modtool interacts with the FSU by sending serial commands to that COM port.  Modtool will try to guess
which COM port your FSU is plugged into but if it cannot do so for some reason (like you have multiple virtual COM ports),
then you will have to specify the correct port.  You can do this by passing a -p <COM PORT or device file> parameter to
modtool.  

To verify that everything is working, attach your MoMo to the computer and run:
```shell
$ modtool list
```

If the command finishes correctly, your MoMo is correctly attached and modtool can find it.  If there is a communication error,
try passing the correct COM port.  On linux this could look like:
```shell
$ modtool list -p /dev/ttyUSB0
```

On Windows it could be:
```shell
$ modtool list -p COM3
```

You will need to replace those port identifiers with the correct ones for your system.  Once you have verified connectivity, 
you can use all of modtool's features to interact with your MoMo device.

### Modtool Commands

#### Listing and Describing MoMo modules
Each MoMo unit is composed of various boards that all perform specific tasks.  You can see what boards are attached to your 
MoMo controller unit by using:
```shell
$ modtool list
```

To get more information about a given module use:
```shell
$ modtool info <INDEX>
```
where INDEX is the index for the module given in the result of the 'modtool list' command.

### Reflashing a Module
If you want to update the firmware code running on an attached MoMo module, you can do so using the modtool reflash command.
You will have to specify which module to reflash using either its name or its address.  You can find the names and addresses
of all the attached modules using the modtool list command.  To reflash a module by address use:
```shell
modtool reflash -a N <HEXFILE>
```

To reflash by name:
```shell
modtool reflash -n gsm_mod <HEXFILE>
```

The reflash command clears the controller's firmware cache to make sure there's room, pushes the hex file to the controller,
finds the module by either its name or address and then sends an RPC call to that module for it to reflash itself.  The command
waits for the reflashing to finish before returning.  Note that since the module cannot communicate during reflashing, the wait
time is just a timer based on the maximum time that the reflashing could take.  You should not remove power from your MoMo unit
or try to use it during the reflashing process.  Doing so will definitely cause the firmware to be programmed incorrectly and
could potentially brick the module.  It is, by design, very hard to brick a MoMo module however so you should be safe.

