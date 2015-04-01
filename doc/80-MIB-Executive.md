# MIB Protocol v2.0
This document describes the design of the revised MIB 2.0 protocol.  MIB is a protocol to allow for multiple processors to talk to share functionality by executing standardized remote procedure calls.  MIB 2.0 extends the previous version of the protocol to support asynchronous RPC calls as well as simplifying the associated implementation code and addressing timeouts and required busy responses.

## Design Goals
- **Asynchronous MIB callbacks** When a slave callback will take a long time, the callee can choose to return an ASYNC status code which is sent back to the caller.  Processing the rest of the command takes place in mainline code and once the result is ready the mainline code calls an API function to send a master message to the original caller with the result in its body

- **Bus Timeout** When a slave is synchronously responding to a MIB message, it can only take a maximum amount of time.  If it
exceeds this time, the master fails the call with a timeout error.

- **Busy Response** When a slave cannot accept another MIB call because it is currently processing an asynchronous call for example, or for another reason, it still returns a busy response so that the caller knows to try again later.

- **Heartbeat** Slaves have a well-known status endpoint that can be used to query whether they are online and functioning correctly.

- **Simplicity** Protocol should be implementable with a very small memory footprint on 8-bit devices and be as stateless as possible to minimize the chances of devices getting into inconsistent or unrecoverable states.

## Wire Protocol
The protocol is built on top of I2C.  Each transaction between two devices is built around an RPC call that always has the same structure.  An i2c master writes the command to the bus, indicating what function it would like executed and which slave should perform the task.  The slave receives and processes the command while clock stretching and then the master reads the response from the slave.  If the command was corrupted in transmission, the master sends it again.  If the response was corrupted in transmission, the master rereads it until it gets a valid response.

A fixed size packet of 25 bytes is used with a 4 byte header, 20 byte payload and 1 byte checksum. This allows for efficient implementation of the master and slave state machines.

### Packet Format

```
-------------------------------------------------
|    RPC Call Packet 	|    Response Packet 	|
-------------------------------------------------
-------------------------------------------------
| 0| Sender				| Return Status			|
-------------------------------------------------
| 1| Flags + Length 	| Checksum		 		|
-------------------------------------------------
| 2| 	 				| Reserved (must be 0) 	|
|  | Command ID			-------------------------
|  | (2 bytes)			| Length				|
-------------------------------------------------
| 4|Parameters 			| Return Value 			|
|  |(Up to 20 bytes)	| (Up to 20 bytes)		|
-------------------------------------------------
|24| Checksum (1 byte)	| Checksum 				|
-------------------------------------------------
```
			
### Master Message Flow
When a device wants to initiate an RPC call, it first waits until the bus is free and then sends an i2c message containing
the information about the call.  The slave executes the call while clock stretching and then the master reads the response
value from the slave.

- Master waits for bus to become free
- Master writes entire packet + checksum
- Master reads 2 bytes
- If bytes indicate no error and that there's a non-void return value then master reads the rest of the 25 byte packet.

<Flowchart here>

### Status Codes
The status code field has 2 special high bits that define how to interpret the rest of the respond and 6 low order bits that give additional information.  

- Bit 7 (MSB) sets whether the response includes more data than just this status code.  If Bit 7 is 0 then the master should not read anymore.
- Bit 6 sets whether the low order bits should be interpreted as having a globally agreed upon meaning or whether they must just be passed onto the application that made the MIB call for processing. Global status codes are things like "Command not found", and "Checksum error on command packet".  If Bit 6 is clear then the low order bits are globally significant otherwise they are only relevant to the specific MIB call that was attempted.

```
-----------------------------------------------------------------
|  7	|	6	|	5		4		3		2		1	 	0 	|
-----------------------------------------------------------------
| Data 	|App Def| Status code (some global, others app defined)	|
-----------------------------------------------------------------
```

### Well-Known Status Codes
- **0b00000000**: Slave is busy.  This code means that the slave exists and is responding to MIB calls but is busy processing another call at the moment and cannot respond right now.  The caller could productively try again later.
- **0b11111111**: Slave does not exist.  This is the response code that is obtained when nothing drives the bus in response to a master's message.  The most likely explanation is that no one has the address that the master tried to talk to.  A less likely but still possible option is that the slave that was being contacted had just switched to master mode to make a callback, lost arbitration and so was unavailable to respond to its own address.
- **0b00111111**: This call was successful but the slave has elected to respond to this MIB call asynchronously
- **0b00xxxxxx**: There was an error processing the mib call.  The 6 low order bits correspond to the error that occurred.  Depending on the error, the master may have to automatically retry the command so some status codes may be defined but not ever returned to the user.
- **0bx1000000**: The call was successful and the slave completed the call synchronously.  The x bit sets whether the slave returned any data
- **0b01xxxxxx**: The call was not successful because the slave endpoint had a problem.  The low order 6 bits correspond to an application defined error code describing the problem.

## Asynchronous callbacks
TODO, finish this section. Async callbacks happen after task code.


## Frequently Asked Questions

### Why is the busy code 0x00?
Since the checksum is the two's complement of the sum of the application bytes, this is the only packet that the slave can send without needing to access its internal state variables.  If the slave is busy, it can't overwrite any data in its MIB buffer because that data is probably being used by a task that its running.  However, sending 0x00, 0x00 requires not state and is a valid response since 0x00 is the two's complement of 0x00.

### Why are all the packets a fixed size?
This is done for implementation efficiency on small 8-bit devices.  By having a fixed size for the response and command packets, it's easy to write shared functions for putting data from the i2c bus into a buffer, computing and verifying checksums of that data.