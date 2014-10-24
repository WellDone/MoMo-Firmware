## Using the GSM module

This tutorial assumes that you have a programmed v5 GSM module and a working connection from your PC to the MoMo.  It will introduce the GSM module and outline steps for setting it up.

### Physical setup

There are a few steps required to prepare the GSM module for transmission:

- Attach an antenna to the u.FL connector.  Make sure the connector is pushed all the way down and try to avoid having the antenna cable traverse over any of the board electorics
- Insert a Micro SIM card into the SIM card slot - this may require cutting a full-sized SIM or using a SIM Card Punch
- Wrap the entire MoMo in paper (to prevent shorting) and then tin foil from the kitchen (to work around a known issue with GSM transmission RF feedback)

### Hardware RF Feedback Known Issue

With version 5 and earlier GSM hardware, there is a known issue which can cause GSM transmissions to fail.  The symptom exhibited is as follows:

When sending a GSM message, the entire MoMo may reset (you can determine that this has happened by looking at the system log) while openning the GSM module attempts to acquire a network signal.

This won't always happen, but it can cause some frustration when it does.  Luckily, there is a relatively simple workaround:

The issue occurs because the GSM module causes excessive RF feedback to overload the circuitry.  This can be avoided, interestingly, by wrapping the entire device in standard kitchen aluminum foil.  To prevent the foil from causing shorts, the MoMo should first be wrapped in a tube of paper so that none of the metal foil comes in contact with the PCB.

### Testing with GSMTool

There is a command-line program called `gsmtool` in tools/bin that can be used to interact with the GSM module.  This may eventually be replaced by the `momo` tool.

The following is the output of `gsmtool -h` which describes some of what the tool can do:

```
Usage:
    gsmtool COMMAND [ARGS...]
    gsmtool help [COMMAND]

Options:
    -h, --help  show this help message and exit

Commands:
    cmd            Tell the GSM modem to execute an AT command
    debug          Get debug info
    dump           Dump the modem's serial buffer (for debugging)
    help (?)       give detailed help on a specific sub-command
    msg            Send a text message with the content <text> to <dest>
    off            Turn off the GSM modem
    on             Turn on the GSM modem.
  ```
  
  To test that the module is set up and working properly, follow these steps. *NOTE* this assumes that the GSM module is at address 11, replace 11 with the address of the module according to the output of `momo controller enumerate_modules`.
  
  1. Turn on the module `gsmtool -a 11 on`, this may take a few seconds.
  2. Check that the module turned on with `gsmtool -a 11 debug`, the output should include `module_on: True`
  3. Run an AT command to check that the SIM card is installed correctly: `gsmtool -a 11 cmd AT+CPIN?`, output should be `+CPIN: READY`
  4. Run another AT command to check that the module was able to connect to the network: `gsmtool -a 11 cmd AT+CREG?`.  If the output is `+CREG: 0,2` then the module is still searching for the network, if it is `+CREG: 0,1` then it has successfully registered.  Anything else probably indicates lack of signal (see below) or a bad SIM card.
  5. Run `gsmtool -a 11 cmd AT+CSQ` to check the signal strength.  A sample ouput is `+CSQ: 31,0` - The second number should always be 0, higher is better for the first number (31 is a strong signal).
  6. Finally, run `gsmtool -a 11 <number> "<message>"` to send an SMS containing the text <message> to <number>.  For example: `gsmtool -a 11 +14155551234 "This is a test of the emergency alert system."`
  7. Check that the message was sent by running `gsmtool -a 11 dump` and noticing that there are no errors, then running `gsmtool -a 11 debug` and noticing that the module turned itself off.

### Setting up autonomous reporting

Sending SMS messages is all well and good, but that was a lot of steps.  Let's set up MoMo to send messages by itself.  The tool we will use this time is affectionately called `reportinator` and is used to configure the reporting settings on the MoMo controller.

The following sequence tells the MoMo to start reporting to +1-415-555-1234 every hour, using the default aggregate settings (Min and Max over the whole interval, Mean and Count for each step)

```shell
$ reportinator route +14155551234
(12) +14155551234
$ reportinator interval 5
5
$ reportinator start
```

### Configuring reporting settings

Running `reportinator -h` yields:

```
Usage:
    reportinator COMMAND [ARGS...]
    reportinator help [COMMAND]

Options:
    -h, --help  show this help message and exit

Commands:
    aggregates     Get or set the report aggregates.
    flags          Get or set the report flags.
    help (?)       give detailed help on a specific sub-command
    interval       Get or set the reporting interval.
    log            Manage the log of reports, succeeded and failed.
    parse          Parse a report in BASE64 format
    route          Get or set the destination address for reporting.
    send           Send a single report
    sequence       Get the current report sequence.
    start          Start reporting
    state          Get the current state of the autonomous reporting flag.
    stop           Stop reporting
    test
```

Here's an overview of the different settings and commands available:

#### Aggregates

The aggregation / reduce functions to run over each time step, as well as over the entire reporting interval.  Use `reportinator aggregates` to read the current settings, and `reportinator aggregates <step_agg> <bulk_agg>` to set them.  Aggregates are reported and set as integer numbers, but internally they are a bitset with the following possible values:

- 0b00000001 = Count
- 0b00000010 = Sum
- 0b00000100 = Mean
- 0b00100000 = Min
- 0b01000000 = Max

Therefore, an aggregate value of 1 reports only the count, a value of 4 reports only the mean, while a value of 99 reports Max, Min, Sum, and Count (that's 64 + 32 + 2 + 1).

#### Flags

These will probably go unused, but can be set to include static values in every report from this MoMo.  `reportinator flags` to read and `reportinator flags <value>` to write.

#### Interval

How frequently a report should be sent.  `reportinator interval` to read and `reportinator interval <value>` to set. Possible values are:

- 1 = Every Second
- 2 = Every 10 Seconds
- 3 = Every Minute
- 4 = Every 10 Minutes
- 5 = Every Hour
- 6 = Every Day (default)

#### Log

Interact with the report log stored in flash on the MoMo controller.  Possible sub-commands:

- `reportinator log read [<n>]` - Read the last `<n>` entries from the log.  If `<n>` is not specified, read all entries.
- `reportinator log count` - Get the total number of entries in the log
- `reportinator log clear` - Erase the log completely (use with caution)

#### Parse

This command does not interract with an attached MoMo, but rather takes a base64-encoded MoMo report as its first argument and outputs a human-readable version, i.e.:

```shell
$ reportinator parse AgwNAAAAugLAIQAAYQUSGMAhAAAAAGkBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAaAEAAGgBAABoAQAAZwEAAA==
version: 2
sensor: 12
sequence: 13
flags: 0
battery charge: 3.79V
diag: 8640, 0
Interval type: hour
         step: 1
        count: 24
Global aggregates:
	count = 8640
	min = 0
	max = 0
Interval Aggregates:
           count    mean
             361       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             360       0
             359       0
```

#### Route

The destination SMS address for reports.  `reportinator route` to read and `reportinator route <number>` to set.  The number should be of the form `+<cc>xxxxxxxxxx`, i.e. `+14155551234`.  No punctuation or spaces, start with + followed by the country code and then the phone number.

#### Send

Artificially construct and send a report, right now.

#### Sequence

Read the current sequence number which will be embedded in the next report.

#### Start

Start sending reports automatically at the designated interval.

#### State

Read whether or not the MoMo is currently generating regular reports (possible results: `Enabled` or `Disabled`)

#### Stop

Stop sending reports automatically.

#### Test

Run a test on report construction functionality on the attached MoMo.


