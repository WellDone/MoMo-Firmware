
# MoMo Modules

## Core Modules
### Controller

This is the brain of any MoMo unit, and is the only module that is absolutely required.  The controller module contains:
- A 16-bit PIC24 MCU.
- 1MB of Flash memory
- A battery connector (which expects 2.8-3.4V DC)
- A battery charging circuit (the charging port expects >100mA from a solar panel or USB charger)

Version 5 of the controller hardware, currently the top of the line, is pictured below.

(image needed)

In this picture, the white connector on the left is for the battery and the port on the right is (optionally) for the charging source.

### GSM Module

This is the first communication module you will encounter.  It contains a quad-band GSM radio and requires a micro-SIM card as well as a uFL antenna.

### MultiSensor

This is the Hulk Hogan of sensor boards.  It can serve many different purposes, and contains circuitry for the following sensors:

- 1 pulse counter
- 3 voltage sensors
- 1 current sensor
- 1 pressure sensor (requires an additional pressure transducer IC component)
- 1 serial comm line

More information about the MultiSensor and how to use it coming soon.

### FSU

This unit is currently used to control, reflash, and debug MoMo units.  It acts as a bridge between the MoMo's MIB communication protocol and USB.

## Building Custom Modules

(Documentation needed)
