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
  6. Finally, run `gsmtool -a 11 <number> "<message>"` to send an SMS containing the text <message> to <number>.
  7. Check that the message has sent by running `gsmtool -a 11 dump` and noticing that there are no errors, then running `gsmtool -a 11 debug` and noticing that the module turned itself off.

### Setting up autonomous reporting

TODO
