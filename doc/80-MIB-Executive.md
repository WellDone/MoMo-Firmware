# The 8-bit MIB Executive
The MoMo system works by dividing work up among different modules that each handle a single task and communicate with other modules to produce complex behavior.  In order to enable this functionality, each module needs to run code that listens for MIB calls on the shared bus as well as allowing the module to make calls itself to other modules.  For any 8-bit enhanced midrange PIC microprocessor, there is a single MIB executive kernel that performs these tasks.  

The executive provides an API for making and receiving MIB calls as well as handling bootloading new application firmware over MIB and making sure that application firmware is compatible and configured properly before being run.  It lives in the low portion of program memory:

```
Program ROM
--------------------
|  MIB Executive   |
|				   |
|  				   |

--------------------
|  Application 	   |
|  Firmware		   |
| 				   |
| 				   |
|				   |
--------------------
|  MIB Block 	   |
--------------------
```

## How It Works
After a reset, the MIB executive runs before jumping to any application code.  It checks to make sure a valid application is loaded by reading its own MIB Block, requests a dynamic address from the bus controller and, if everything checks out, it jumps into application code.  

The MIB executive also intercepts all interrupts on the device and automatically handles those related to MIB functionality: I2C related interrupts and interrupt-on-change notifications on the MIB ALARM pin.  All other interrupts are passed on to the application module to handle or ignored if there is no valid application loaded.

## Resource Usage
The MIB executive is very lightweight. It takes only XX bytes of RAM in order to provide a buffer for sending and receiving MIB calls and a few state variables.  There is almost no runtime overhead except when the executive is active and processing a MIB call.  The executive uses the first XX bytes of RAM in bank 0, leaving all other ram for the application module.

## Executive State Variables
All of the executive state is contained inside a single 2-byte state variables, with each bit assigned to a specific task.

```
Executive State
--------------------|
|00| Respond Busy	|
--------------------|
|01| Valid App		|
--------------------|
|02| Async Callback	|
--------------------|
|03| Registered		|
--------------------|
|04| Dirty Reset	|
--------------------|
|05| Master Waiting	|
--------------------|
|06| First Read		|
--------------------|
|07| Trapped		|
--------------------|
```

### Bit Meanings

0. **Busy** All RPC calls sent to the module will be ignored and a busy signal will be returned.  This signal is distinct from the response given by a module that doesn't exist, so callers know that the module exists and is busy.
1. **Valid App** Set during executive startup to indicate that a valid application has been loaded (including having the correct magic number and checksum to protect against corruption) 
2. **Async Callback** Set when an RPC slave handler on this device returns `kAsynchronousResponseCode`, triggering the executive to enter asynchronous response mode until mainline code finishes sending the asynchronous response.
3. **Registered** Set when the executive succesfully contacts a controller during startup and receives an address.
4. **Dirty Reset** A safety flag that is set before jumping into application code for the first time.  Upon device reset, this bit is checked to see if it is set and if so, the executive calls `trap()` for debugging.  This lets us know anytime the device resets without calling the api function `device_reset()` which clears this bit before calling the the `reset` instruction to actually reset the device.
5. **Master Waiting** Set when mainline code made an rpc call using `bus_master_rpc_send(uint8_t param_length)` and the slave elected to respond asynchronously.  Mainline code blocks polling this bit until it is cleared upon reception of the asynchronous response.
6. **First Read** Set when a slave RPC is begun (upon receipt of our address with write indication over I2C).  Once the slave handler is successfully called, this bit is cleared so that subsequent attempts to read the slave's response packet do not trigger repeated handler execution.
7. **Trapped** Set when the `trap(uint8_t reason)` API call is executed, causing mainline code to block and wait to be debugged.

## Handling MIB Master - Slave Transitions
The executive processes all MIB slave call in interrupt context, leaving mainline code only for making master calls.  However, this means that some coordination needs to happen when an application wants to make a master MIB call because it has to make sure that it doesn't deadlock with the slave call waiting for some mainline code to run while the mainline code is blocked waiting for the MIB bus to become free.  

So, whenever the application code wants to send a MIB master request, it needs to first request control of the bus from the slave by calling `bus_master_begin_rpc`.  This API call first waits for the bus to become idle by calling `bus_is_idle` and then transitions the bus over to master mode.  At this point the mainline code takes control of the bus and does not allow any other slave calls to be processed until it finishes sending its MIB message.  

**NB** Due to the nature of the I2C hardware in the PIC12 and PIC16 series, the module can only be in either master or slave mode at any given time, so once `bus_master_begin_rpc` returns, all subsequent slave RPC calls received will be ignored as if the module was nonexistent until `bus_master_send_rpc` returns.

## Responding to MIB Calls Asynchronously
If a MIB callback is received that would take a long time to process, such as connecting to a website over cellular data, where you may have to turn the modem on, wait for a connection, etc, the handler can elect to respond asynchronously.  The handler does this by returning a special return value `kAsynchronousResponseCode`.  When a handler returns asynchronously, the following actions are taken by the executive:

- `Async Callback` bit is set.
- The handler returns and mainline code execution is resumed.
- Whoever sent the RPC call that we are processing asynchronously will receive an Asynchronous Status response with no data.
- If anyone else tries to send us an RPC call before we finish responding to this one then, upon receiving our address, we will set the `Busy` bit so that we do not disturb the contents of the MIB buffer.  All subsequent RPC calls received will be responded to as busy until the asnychronous call is finished by mainline code.

It is the responsibility of the mainline application code to see an internal application flag set by the initial MIB handler, take the appropriate action and then call `bus_master_async_callback` once the appropriate response is ready.  Internally, `bus_master_async_callback` just makes a special master call back to the original caller in order to inform them about the result of their MIB call.

Inside `bus_master_async_callback`, the `Async Callback` bit and `Busy` bits are both cleared.