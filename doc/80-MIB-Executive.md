# Asynchronous MIB Upgrade

## API Calls
- 

## Required Features
- **Asynchronous MIB callbacks** When a slave callback will take a long time, the callee can choose to return an ASYNC status code which is sent back to the caller.  Processing the rest of the command takes place in mainline code and once the result is ready the mainline code calls an API function to send a master message to the original caller with the result in its body

## Wire Protocol
A fixed size packet of 25 bytes is used with a 4 byte header, 20 byte payload and 1 byte checksum. 

## Packet Format
Feature 					Return Status
Command 					Checksum
Sender 						Reserved (must be 0)
Type+Length 				Length

Parameters (N bytes)		Return Value
Checksum (1 byte)			Checksum
							
							

## Master Message Flow
When a device wants to initiate an RPC call, it first waits until the bus is free and then sends an i2c message containing
the inforation about the call.  The slave executes the call while clock stretching and then the master reads the response
value from the slave.

- Master waits for bus to become free
- Master writes entire packet + checksum
- Master reads 2 bytes
- If bytes indicate no error and that there's a response then master reads the rest of the 25 byte packet.

## Slave Response

Return status possibilities:
|7		|	6			|	5		4		3		2		1	 	0 	|
|Okay	|	Has_data	|	Status code (some global others app defined)|

0b00000000		- busy status code: the device was not able to respond because it is processing another request
0b00xxxxxx		- there was an error processing this call (bit 5 indicates if app handler was called, bits 0-4 indicate the error code)
0b10xxxxxx		- call was successful but no freeform data was returned (bits 0-5 indicate the return code)
0b110xxxxx		- call was successful and freeform data was returned (bits 0-5 indicate the return code)
0b11111111		- slave did not respond at all to command (indicates slave address was not taken or slave lost arbitration)

## Asynchronous callbacks
When a master call makes a 