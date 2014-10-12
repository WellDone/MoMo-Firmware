# WellDone MoMo Hardware and Firmware

A product of [WellDone.org](http://www.welldone.org).

[![Build Status](https://travis-ci.org/WellDone/MoMo-Firmware.svg?branch=dev)](https://travis-ci.org/WellDone/MoMo-Firmware)

Documentation available at http://welldone.github.io/MoMo-Firmware

## Directory structure breakdown ##

* **config** - Configuration for locating tool dependencies
* **doc** - Documentation and component datasheets
* **[momo_modules](./momo_modules)** - This is where the firmware code for individual modules (devices implementing the [MIB](http://github.com/WellDone/MoMo-Firmware/wiki/Module-Interconnect-Bus-(MIB)) protocol over I2C) lives
* **pcb** - This is where the PCB board schematics and other miscellaneous hardware information lives.
* **tools** - Various tools that are useful for building and interacting with a MoMo

## Contributing ##

See the [Getting Started](https://github.com/WellDone/MoMo-Firmware/wiki/Getting-Started) wiki page for a thorough introduction to development of the MoMo platform.

Development can be done locally (see [Development Dependencies](https://github.com/WellDone/MoMo-Firmware/wiki/Development-Dependencies)) or using [Vagrant](http://www.vagrantup.com/) by simply running `vagrant up && vagrant ssh`

## License ##

MoMo designs are released under the [CERN Open Hardware License v.1.2](http://www.ohwr.org/attachments/2388/cern_ohl_v_1_2.txt)

MoMo firmware and software tools are released under the [MIT license](http://opensource.org/licenses/MIT)
