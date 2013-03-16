## Source Directory Structure Guidelines

Source code sub-directories are categorized by the abstraction level and functionality goal of the code that lives in each directory.

* `Core` - This is where the core functionality lives.  This includes all top-level functionality and app-wide shared state/#define headers.
* `Debug` - This is code that deals directly with the processing of commands when the device is connected to a debugger / UART device.
* `Diagnostics` - Here there be monsters.  This is for self-test code and the like.
* `Modules` - Everything .h/.c pair in Modules is responsible for managing a single, discrete hardware component.
* `Tasks` - This is where task loop class definitions will go.
* `Util` - As the name suggests, this is for pure-software utility functions.
* `main.c` - The program entrypoint of course.  This should remain as lightweight as possible.
