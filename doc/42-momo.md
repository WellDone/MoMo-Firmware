## momo (the command line tool)

```momo``` is a powerful command line tool that can be used to explore the pymomo python API and to interact with the momo hardware.  It is the most basic way to interact with the MoMo ecosystem.  The ```momo``` tool is based on the concept of hierarchical contexts which have one or more methods that can be called in that context.  This section details how to understand and use the ```momo``` tool at a basic level as well as advanced customization and usage at the end of the section.

### Quick Introduction
The python API that is used to interact with the momo hardware consists of a number of classes, some of which are proxy objects for hardware devices and others represent types and data on the computer.  As an example, consider interacting with a MoMo controller module.  There is a python class ```MIBController``` that represents the controller connected to the computer and internally converts all method calls on that object into RPC calls that are sent to the controller hardware and the response is presented as the result of the function call.  This lets you control the MoMo hardware as if you were just programming with a standard python object.  

The momo tool has two key concepts: contexts and methods.  A context is a container that has a number of methods.  So ```MIBController``` is a context.  Each context has a number of methods that it can execute.  When you run the momo tool, it lets you call any context's method from the command line.  Internally all of the arguments you pass on the command line are converted to python objects and the method (which is just a regular python method) is called with the result printed for you.  This provides an easy way to interact with any object in the pymomo API without needing to write a python script.  You can just write commands with the momo tool and have them automatically converted into python calls.

When you're ready to write your own scripts, there is a direct 1:1 mapping between ```momo``` commands and python statements, so it's trivial to explore the MoMo ecosystem using the ```momo``` tool and then write more advanced scripts in python once you get the hang of everything.  The system that makes this possible is ```typedargs```, a python framework for annotating functions and classes with additional information that lets the momo tool convert command line strings into typed python objects and validate that the data entered makes sense.

If you have a MoMo controller plugged into your computer, we can do an example to demonstrate.

```
> momo
(root) 
```

Upon executing the momo command with no arguments you're dropped into the root context and momo creates a REPL (Read-Eval-Print Loop) for you to work in.  You can explore what commands are available using tab completion.  Pressing tab twice shows all of the available commands in this context (powered by GNU readline so it works the same as in the normal shell).

```
(root) <TAB><TAB>
ControllerBlock  Simulator        build            help             quit       
HexFile          back             controller       import_types     
(root) 
```

These are the commands that are available to you at the root level.  To see more details, use the builtin ```help``` command:
```
(root) help
root

Defined Functions:
 - Simulator(string type)
   Generic interface for simulating embedded microprocessors.

 - import_types(path package, string module=None)
   Add externally defined types from a python package or module

 - controller(string serial='')
   Given serial port descriptor, create all of the necessary

 - build(args)
   Invoke the scons build system from the current directory, exactly as if 

 - ControllerBlock(path source)
   The metadata block in a pic24 controller firmware image.  This includes

 - HexFile(path source, integer wordsize, integer filewords, integer skip=1, integer cutoff=65535)
   A generalized container object for representing sparse memory views like those


Builtin Functions
 - quit: quit the momo shell
 - help [function]: print help information about the current context or a function
 - back: replace the current context with its parent
```

```help``` without any arguments prints short documentation on all of the available methods in this context.  To learn more about any particular method, use ```help <method>```.

```
(root) help controller

Given serial port descriptor, create all of the necessary
object to get a controller proxy module 

Arguments:
 - serial (string): the serial port to use
```

Notice how you see a docstring for the method drawn directly from the pymomo package as well as descriptions and types for any arguments that it accepts.

Now lets start interacting with the controller board.

```
(root) controller
(MIBController) help

MIBController
A python proxy object for interacting with the MoMo controller board.

Defined Functions:
 - reset(sync=True)
   Instruct the controller to reset itself.

 - current_time()
   Get the current time according to the controller's RTCC

 - sync_time()
   Set the time on the momo controller to the current system time.

 - read_log()
   Read the MoMo system log

 - enumerate_modules()
   Get list of all attached modules and describe them all

 - build_report()

 - clear_log()
   Clear the MoMo system log.

 - get_module(string name=None, integer address=None, force=False)
   Given a module name or a fixed address, return a proxy object

 - perf_counter(integer counter)

 - count_modules()
   Count the number of attached devices to this controller

 - get_report()

 - factory_reset()
   Instruct the controller to reset all internal state to "factory defaults."  Use with caution.

 - set_time(year, month, day, hours, minutes, seconds, weekday=0)
   Set the current time of the controller's RTCC

 - read_ram(integer address, string type=None, integer size=0)
   Read bytes from the controller ram starting at the specified address.  If a 


Builtin Functions
 - quit: quit the momo shell
 - help [function]: print help information about the current context or a function
 - back: replace the current context with its parent
```

Notice how the context changed from root to MIBController.  From now on any method we execute will be one defined in the MIBController class.  If you're done with a context and want to go back, use the builtin function ```back``` to destroy the current context and go back to its parent.  Technically, the list of contexts you've visited forms a stack so you can always pop the top context off and go back to the one before it using the ```back``` command.  

If you type back from the root context, ```momo``` will exit since there's no context to go back to.  Otherwise you can quit by typing ```quit``` in any context since its a builtin function that's always available like ```back``` and ```help```.

The core concept of ```momo``` is that if you know the python method you immediately know how to call it in ```momo``` and if you call a method using the ```momo``` tool you also know how to call it in python since the two are always linked.  The purpose of the ```momo``` tool is to provide an easy way to interact with the pymomo API without having to write python scripts for everything.

### Using momo without dropping into a shell

If you just want to type a quick command, you can do so without first creating a shell by entering the command into the initial command line used to invoke ```momo```.  For example, to reset a connected controller board just use:

```
> momo controller reset
```

Internally, momo parses the command line piece by piece so first it enters the controller context and then calls the reset method.  If the last method invoked does not create another context, the momo tool terminates since this is likely what you wanted.  If on the other hand, the last method does create a context then momo drops you into a shell in that context.  This lets you speed up access to contexts that you want to interact with for some time but require several commands to setup.  For example:

```
> momo controller
(MIBController) 
```

You can chain as many methods together as you like, so if you want to pull a system log from a MoMo controller and display it you could do:
```
> momo controller read_log display 
PERFORMANCE (2000-01-31 16:30:20) Periodic log of how often the microprocessor is asleep
 - Total Ticks: 23110
 - Sleeping Ticks: 22370
 - Number of prevented sleeps: 0
 - Tasks: 0x2356
CRITICAL (2000-01-31 16:30:20) The MoMo controller received a reset RPC and is resetting
CRITICAL (2000-01-31 16:30:20) The MoMo controller has been reset
CRITICAL (2000-01-31 16:30:20) The MoMo controller has been initialized
CRITICAL (2000-01-31 17:23:32) The MoMo controller received a reset RPC and is resetting
CRITICAL (2000-01-31 17:23:33) The MoMo controller has been reset
CRITICAL (2000-01-31 17:23:33) The MoMo controller has been initialized
PERFORMANCE (2000-01-31 17:23:40) Periodic log of how often the microprocessor is asleep
 - Total Ticks: 27576
 - Sleeping Ticks: 26890
 - Number of prevented sleeps: 0
 - Tasks: 0x2356
```

```momo controller read_log``` pulls the system log from the MoMo controller board and creates a SystemLog context with the information it receives.  This context supports the display method to show its contents, which is then invoked.  

### Running Commands on Startup

You'll likely find that you want to set up the momo tool to always initialize certain contexts everytime you create them.  For example, the SystemLog context requires commands adding LogDefinition files so that it knows how to parse the data that it's getting back from the MoMo controller.  The actual command that was executed in the last example was:

```shell
> momo controller read_log add_ldf mainboard/src/log_definitions.ldf add_ldf shared/pic24/src/log_definitions.ldf display
```

However, this is tiresome to write.  So you can create a configuration file that contains momo commands that are run whenever a certain context is created.  The format of the file is straightforward:

```
> cat ~/.config/momo_config.txt
[root]
import_types /home/timburke/Projects/MoMo-Firmware/momo_modules/shared/pic24/python/firmware_types

[MIBController.SystemLog]
add_ldf /home/timburke/Projects/MoMo-Firmware/momo_modules/mainboard/src/log_definitions.ldf
add_ldf /home/timburke/Projects/MoMo-Firmware/momo_modules/shared/pic24/src/log_definitions.ldf
```

The file should be called momo_config.txt and stored under the .config directory in your home folder on unix and in %APPDATA% on windows.  To Find where the config file should be stored on your system just run:

```
> momo --rcfile
/Users/timburke/Library/Preferences/momo_config.txt
```

The example shows the directory location on a Mac OS X system.

On all systems, the format of the momo_config file is the same.  It consists of a list of context patterns in brackets followed by initialization commands.  Whenever a context matching the pattern is created, the initialization commands are run.  A context pattern can be either a single context name like root for the root context or a hierarchy of contexts separated by dots.  For example, above we want to run a few add_ldf commands everytime a SystemLog is created by a MIBController.  If the SystemLog is instead created by another context, we don't want to modify it.

Adding commands under the root context will cause them to always be run when you invokve the ```momo``` tool.

### Adding Additional Types
If you create your own MoMo hardware modules or get them from other people, there will likely be additional proxy objects and types that will be useful for interacting with those modules but not contained in the core pymomo package.  The root context of ```momo``` has a method ```import_types``` that imports arbitrary types and makes them available for use in the momo tool.  It's usage is beyond the scope of this section though and will be detailed elsewhere.
