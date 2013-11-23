# WellDone MoMo Firmware

A product of [WellDone.org](http://www.welldone.org).

Before contributing, study the enforced Git branching model on the [Wiki](http://github.com/WellDone/MoMo-Firmware/wiki).

## Directory structure breakdown ##

* **doc** - Documentation on the status
* **[momo_modules](./momo_modules)** - This is where the firmware code for individual modules (devices implementing the [MIB](http://github.com/WellDone/MoMo-Firmware/wiki/Module-Interconnect-Bus-(MIB\)) protocol over I2C) lives
* **pcb** - This is where the PCB board schematics and other miscellaneous hardware information lives.
* **tools** - Various tools that are useful for building and interacting with a MoMo

## Build System ##
MoMo firmware is moving toward the SCons build system for build management and unit testing