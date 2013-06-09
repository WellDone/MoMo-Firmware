### Code for individual MoMo component modules

All modules must implement the [Module Interconnect Bus (MIB)](http://github.com/WellDone/MoMo-Firmware/wiki/Module-Interconnect-Bus-(MIB\)) over I2C.

* `mainboard` (pic24) - The main controller module
* `field_service_unit` (pic24) - The USB<->I2C bridge used to service MoMos in the field.

### Shared Code

* `shared/picXX` - This code is specific to the picXX (i.e. pic12 or pic24) processor, but does not contain code specific to an individual module.
* `shared/portable` - This code (and anything it includes) should be able to run on EITHER an 8-bit pic12 or a 16-bit pic24.

### Source Directory Structure Guidelines

Source code sub-directories are categorized by the abstraction level and functionality goal of the code that lives in each directory.

* `Core` - This is where the core functionality lives.  This includes all top-level functionality and app-wide shared state/#define headers.
* `Debug` - This is code that deals directly with the processing of commands when the device is connected to a debugger / UART device.
* `Diagnostics` - Here there be monsters.  This is for self-test code and the like.
* `Modules` - Every .h/.c pair in Modules is responsible for managing a single, discrete hardware component.
* `Util` - As the name suggests, this is for pure-software utility functions.
* `main.c` - The program entrypoint of course.  This should remain as lightweight as possible.
