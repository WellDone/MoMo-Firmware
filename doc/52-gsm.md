## Using the GSM module

This tutorial assumes that you have a programmed v5 GSM module and a working connection from your PC to the MoMo.

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
