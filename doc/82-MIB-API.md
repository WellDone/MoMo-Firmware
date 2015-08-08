## MIB12 API Reference
Application modules that run under the mib12_executive interact with other mib modules using API functions provided by the mib12_executive.  The functions are defined in the shared pic12 library in the files:

- `mib12_api.as` Contains the symbol definitions so that the linker can appropriately generate call instructions.
- `mib12_api.h` Contains function definitions for use in C applications. 

This document provides reference information on the available API functions.  It pertains to **Version 1.1** of the mib12 API.

### API Functions
There are currently 6 API functions

`void bus_master_begin_rpc(uint8_t destination)`

Take control of the bus and begin a master RPC call to the module at address destination.  If the bus is busy, this function blocks until it is idle and then switches the I2C hardware to master mode.  You must not write anything to the `mib_buffer` before calling this function since you could otherwise trample on parameters being received from another module in the process of making an RPC call to your application.  After calling `bus_master_begin_rpc`, you should fill the `mib_buffer` with your parameters and then call `bus_master_send_rpc` to actually send the RPC call.  This function must be called from mainline code only.  If called from interrupt code it may block forever.

`uint8_t bus_master_send_rpc(uint8_t parameter_length)`

Actually send the RPC that was started by a previous call to `bus_master_begin_rpc`.  The single parameter is the length of the valid data written to `mib_buffer` containing the parameters for the call.  You must also choose the RPC command id that you would like to call on the remote device as shown in the example below.  The command ID is not part of the mib_buffer and its length should not be included in the `parameter_length` passed to this function.  Upon calling the function, the mib12_executive blocks until the command either completes successfully or a fatal error occurs.  The function returns a status code indicating what happened.  The possible outcomes of this function are:

- Successfully communicated with the remote module, called the RPC and the results are now in the mib_buffer.  The return value of this function is application defined by the remote RPC call.
- Successfully communicated with the remote module, but there was no RPC that matched what you wanted to call in `command`.  The return value of the function is `kCommandNotFoundStatus`.
- Successfully communicated with the remote module, but it is busy and cannot process RPC calls right now.  The return value of the function is `kModuleBusyStatus`.
- The remote module does not exist or is in master mode and cannot receive RPC calls right now.  The return value of the function is `kModuleNotPresentStatus`.

RPC packets are checksummed on the wire and any transient communication errors are automatically corrected during the execution of `bus_master_send_rpc`.  When this function returns, either the RPC call was successful, or it was impossible.  This function must only be called from mainline code.  If called from interrupt code it will block forever.

`void bus_master_async_callback(uint8_t result_length)`

Finish a previous asynchronous RPC call that was sent to this module.  This function should only be called after the module receives an RPC call as a slave and chooses to response with `kAsynchronousResponseStatus`, meaning that it will send the actual response at a later time.  When the module is ready to send the response, it must call this function from mainline code after loading the `mib_buffer` with all of the data for the response.  It is *not required* to call `bus_master_begin_rpc` before calling this function and indeed it is forbidden to call `bus_master_begin_rpc` or `bus_master_send_rpc` when an asynchronous RPC call is pending.  Internally this function executes a master rpc call back to the original RPC caller to report the result of their RPC request.

`void trap(uint8_t reason)`

Trap an error by looping forever so that a debugger can be connected.  This function must only be called from mainline code so that RPC processing still proceeds correctly.  It is just an infinite loop but it stores the `reason` parameter in a safe place so that you can retrieve it later using an RPC call to debug the issue.  This function also sets a `Trapped` status bit so that people who query the status of this module will see that it has locked up and know to take corrective action.

`void device_reset(void)`

Reset the device.  This function can be called from either interrupt or mainline code and will execute a soft-reset of the device.  Anytime you want to reset the device, you must call this function rather than executing a `reset` instruction directly, otherwise the mib12_executive will detect the dirty reset and call `trap` upon resetting.

`void bus_slave_returndata(uint8_t length)`

Set the length of the data returned by a slave handler.  This function should only be called in interrupt context inside of a MIB RPC handler to indicate how much data it being returned.  If no data is being returned, it *does not* need to be called with an argument of 0 since the return length is initialized to 0.  It may be called multiple times during a single RPC handler without problems but only the last call will have any effect. 

### MIB Master RPC Call Example
This example illustrates how to correctly make a MIB master RPC call.  Say you want to call RPC number 0xAABB on module address 12 and pass in 5 bytes of parameters.  You would do:

```c

//Begin an RPC to address 12
bus_master_begin_rpc(12);

//Load in parameters
mib_buffer[0] = 1;
mib_buffer[1] = 2;
mib_buffer[2] = 3;
mib_buffer[3] = 4;
mib_buffer[4] = 5;

//Load in the command ID (this is a macro)
bus_master_prepare_rpc(0xAA, 0xBB);

//Send 5 bytes of parameters
uint8_t status = bus_master_send_rpc(5);

if (status == kNoErrorStatus)
	foo(); 
else
	bar();
```

### MIB Slave RPC Handler Example
Let's say that you are creating a slave handler that responds to requests for a 2 byte magic number for anyone who asks.  You would write the handler as follows"

```c

//Define a slave handler that returns a 2 byte magic number
uint8_t respond_magic_number(void)
{
	mib_buffer[0] = kMagicNumberLow;
	mib_buffer[1] = kMagicNumberHigh;

	bus_slave_returndata(2);

	return kNoErrorStatus;
}
```
