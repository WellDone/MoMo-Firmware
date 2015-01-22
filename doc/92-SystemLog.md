# Understanding and Using the MoMo System Log
Each MoMo controller board has a nonvolatile system log that is used to log debug and error information as well as (in the future) for running hardware tests.  The log is stored in the external flash memory on the controller board as a circular buffer so, if it fills up, the oldest entry will be overwritten.  This document details how the system log is structured as well as how to interact with it and use it programmatically.

In order to be fast and general, log message strings are not embedded into the system log, instead a 32-bit hash is generated as a unique ID for each log message and that id is stored in the log.  Parsing the log back to string messages rather than message IDs requires having access to the Log Definition File that was used to compile the firmware code that emitted the log message.

## System Log Format
The system log is composed of a series of typed entries (RawEntry) of a fixed size (currently 16 bytes).  A log entry starts with a RawEntry containing the timestamp (accurate to the second) of the logging message as well as a 32-bit id number that corresponds with the message being logged.  It is followed by zero or more RawEntries that correspond with parameters that are associated with the log entry.  If parameters are too big to fit in a single RawEntry, they are automatically split across as many RawEntries as necessary.  Integer (16-bit) parameters are packed into a single RawEntry to save space when possible (7 per entry since 1 byte is taken up with entry type information).

### Parameter Types
Currently the supported parameter types are integer, boolean, string and blob.  Blob simply indicates that the data should be interpreted an an opaque array.  The corresponding log definition for this entry can specify how to interpret the blob and display its logical internal structure in a useful way.

## Using the System Log Programmatically (controller)
From controller code, which has direct access to the system log without making an RPC, you can use the following logging macros to log information.  First, make sure you include the appropriate headers:

```c
#include "system_log.h"
#include "log_definitions.h"
```

```log_definitions.h``` is a header file generated during the compilation process that contains a map between log statements and the 32-bit hash ids that are actually stored in the system log.  You should not edit it directly but rather edit the log_definitions.ldf file that is used to generate it.  

To log a message simply use:

```c
LOG_CRITICAL(message_id);
```

There is a also a corresponding ```LOG_DEBUG(message_id)``` macro that can be used.  If you wish to attach parameters to the log statement you can do so by following the initial LOG_**STREAM** statement with an arbitrary number of parameter log statements, for example:

```c
LOG_INT(10);
LOG_POINTER(ptr);
LOG_STRING("Hello world.");
LOG_ARRAY(array, length);
LOG_BOOL(true);
```

When the system log is read, parameter log statements are implicitly associated with the last message_id.

## Using the System Log Programmatically (other modules)
Remote logging from other modules is not yet supported by the system log though support is planned for this feature.

## Creating New Log Entry Types
Every log entry in the system must correspond with a log definition in a Log Definition (.ldf) file.  The format of this file is straightforward and just lists the name of the log statement, the message that corresponds with it, and any parameters that it expects.  The format is:

```
LogStatementName
"quoted string message"

- "Parameter name" [list of] type[, format format_name]
<zero or more parameter statements> 
```

An example with and without parameters is:

```
PowerOnNotice
"The MoMo controller has reset due to a power-cycle event"

DeviceResetNotice
"The MoMo controller has been reset"
- "Welcome mat" as string
- "Welcome number" as integer, format hex
```

The type should correspond to a known type in the typedargs type system and is used to interpret the binary data logged with the entry.  If format is 
specified, it should correspond to a defined formatting routine for the given type.  If no format is specified, then the default formatter for the type
is used. 

You can also define a log statement to contain a list of parameters with variable length but common type.  For example, to log the list of all tasks in the controller taskloop structure you could define a log entry as follows:

```
TaskLoopFullError
"The taskloop was completely full and a new task was added"

- "Tasks" as list of fw_task
```

The special syntax ```list of <type>``` defines this entry to consume all remaining logged parameters.  You can have at most one list per log entry and it must be the last parameter.  If you create an LDF file violating this rule, the build system will throw an error to let you know.

###Important Notes

- The 32-bit message id is formed from the low 32 bits of the MD5 hash of the LogStatementName, so if you change the LogStatementName, it will no longer hash to the same value and hence the system log reading functions will not be able to match it with the hash value stored in the system log.  *You should avoid* changing the name of existing log entries and instead just create new log entries.
- There is a very small, though nonzero probability that two Log Statement Definitions collide to the same hash value.  If this happens, the build system will throw an exception and you will have to modify the name of one of the definition names in order to fix the collision.  With 4 billion choices, this should not be too much of a problem.

## Reading the System Log
Using the momo tool is the easiest way to read the system log.  The most basic option would be to connect the momo to the computer and then use:

```shell
> momo controller read_log display
CRITICAL (2000-01-31 16:02:14) 0xF520EAFE
CRITICAL (2000-01-31 16:02:14) 0x67F94DC9
 - Hello world.
 - 10
CRITICAL (2000-01-31 16:02:14) 0x17CE2635
```

You can see each log enty with its stream as well as any parameters with which it was logged.  The log statement is displayed as a hex hash value because you haven't added any ldf files with which to interpet the log.  You can do so interactively by dropping into a momo shell:

```shell
> pwd
/home/timburke/Projects/MoMo-Firmware/momo_modules/mainboard
> momo controller read_log
(SystemLog) display 
CRITICAL (2000-01-31 16:02:14) 0xF520EAFE
CRITICAL (2000-01-31 16:02:14) 0x67F94DC9
 - Hello world.
 - 10
CRITICAL (2000-01-31 16:02:14) 0x17CE2635
(SystemLog) add_ldf src/log_definitions.ldf
(SystemLog) display
CRITICAL (2000-01-31 16:02:14) The MoMo controller received a reset RPC and is resetting
CRITICAL (2000-01-31 16:02:14) The MoMo controller has been reset
 - Welcome mat: Hello world.
 - Welcome number: 0xA
CRITICAL (2000-01-31 16:02:14) The MoMo controller has been initialized
```

Each module that contains logging statements should include a file named log_definitions.ldf in the src directory.  This is the file that should be included using the add_ldf directive. Note how the ldf file both lists the appropriate message to use with the log statement as well as how to interpret and display the parameters.  If you want to do this in one line you can use:

```shell
> momo controller read_log add_ldf src/log_definitions.ldf display
CRITICAL (2000-01-31 16:02:14) The MoMo controller received a reset RPC and is resetting
CRITICAL (2000-01-31 16:02:14) The MoMo controller has been reset
 - Welcome mat: Hello world.
 - Welcome number: 0xA
CRITICAL (2000-01-31 16:02:14) The MoMo controller has been initialized
```

You can add as many ldf files as you like in order to interpret all of the logging statements in the system log.  By default, if there is no log definition available, momo just diplays the raw parameters and the hash of the message id.