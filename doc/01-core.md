# Module Interconnect Bus (MIB)
The Module Interconnect Bus is a shared remote procedure call type communication bus allowing for communication between different modules in the MoMo ecosystem. 

##Physical Specification
The MIB is an i2c bus.  Each module PIC will be connected via SCL and SDA lines that are open drain with pull up resistors on the controller board only.  The i2c bus will run at 100 KHz.  

##Message Structure
The i2c bus has a master-slave format.  The master controls SCL and sets the direction of data transfer.  It can either send or receive data from the slave.  Each complete message in the MIB protocol will correspond logically to a remote procedure call (RPC) where the master is executing a procedure on the slave and getting the return result.  This is summarized in Figure 1.

![](http://github.com/WellDone/MoMo-Firmware/wiki/MIB-Transaction-Flow.png)
Figure 1

The master begins the transaction with the slave by sending an i2c start condition with the slaves address and write indicated.  It sends a packet specifying the feature and command, which uniquely determine the function the slave should execute.   At this point the slave searches for a handler function to handle that command and clock stretches until it has found one or determined that it does not support the command.

The master then sends another packet with the typed parameter list for the command.  The slave receives these and stores them in a buffer specified by the command handler found for the command.  The master then sends a repeated start with read indicated to read the return value for the command.  Upon receipt of this, the slave begins executing the command and clock stretches until it has finished, at which point the master reads at least one and up to two packets specifying the status of the executed command and its return value if there is one.

The master then sends a write packet with at least 1 byte to reset the slave according to the bus Slave Reset Protocol.

The master then sends an I2C stop condition to terminate the communication and both master and slave return to an idle state to await the next communication. 

##Packet Structure
All messages that go onto the wire are composed of packets.  A packet is a series of bytes beginning with an address and ending with a checksum.   It has the following format.  
```
Start or Repeated Start
Address + RW
Data byte 1
…
Data byte n
Checksum
```

###Checksum
The packet checksum is calculated as the 2’s complement of the sum of the data bytes in the packet, not including the address ignoring overflow.  The sum is calculated by using an unsigned 8 bit accumulator value initialized to 0 at the beginning of the packet transmission.  As each byte is transmitted over the wire it is added to the accumulator, ignoring overflow.  After the last data byte is transmitted, the accumulator is negated and 1 is added to it to form the two’s complement, which is then transmitted as the final byte of the packet.  

So a packet with N data bytes will transmit N+2 bytes (address + N data bytes + checksum).  

###Acknowledge Bit
The i2c protocol requires that an acknowledge (ACK) or not acknowledge (NACK) to be transmitted after each byte is transmitted on the wire.  What this bit means depends on the direction of the message.

####Master Writing to Slave
When the master writes 8 bits to a slave, it generates a 9th clock pulse for the slave to send it an acknowledge bit.  The slave should always send ACK.  ACK means that it received 8 bits, not that it knew what to do with them or that they were the bits it was expecting.  This is because the pic24 hardware i2c logic automatically acknowledges received bytes and only sends NACK if the receive buffer is full.  

####Master Reading from Slave
When the master reads from the slave, it generates 8 clock pulses to read 8 bits from the slave and then generates a 9th clock pulse to send an ACK/NACK bit to the slave.  It is very important that the master ACK every data byte from the slave and NACK the checksum byte.  This is because ACK for master reads means that the slave should continue sending bytes.  NACK means that the slave should stop sending bytes.  If the master does not NACK the last byte in the packet (the checksum byte always), the slave will think it should transmit another bit and will in some cases keep pulling SDA low waiting for a clock pulse.  This does not allow the master to control SDA and send a stop condition, causing a bus fault. 

##Protocol Layers
The MIB protocol can be thought of as composed of distinct layers: the i2c physical layer, the packet layer and the RPC layer.  The i2c physical layer controls bus access, and sends or receives bytes from the wire.  The packet layer sends opaque data packets + checksums and verifies the checksum.  The RPC layer composes packets and defines the overall flow of the transaction.

##Parameter Data Structure and Type
MIB supports the following logical parameter types: integer (8 or 16 bit), string and opaque buffer.  On the wire, there are only two wire parameter types (integer and opaque buffer).  The 4 logical parameter types are encoded into these two types in the manner described below.  

##Automatic Recovery from Transient Bus Errors
Every packet in the MIB protocol has a checksum so that the recipient can be sure it has been received without error.  However, this is not an error correcting code, so the recipient must ask the sender to resend the data until it is received correctly.  There are 4 points in the MIB protocol where data integrity is checked:
* When a command is sent (master->slave)
* When all parameters are sent (master->slave)
* When the command status is read (slave->master)
* When the return value is read (slave->master)

Note that the slave does not execute the command handler until it has verified that the command and parameters were received correctly so, if there is a checksum error from master->slave in either the command or the parameters, the master will send a Slave Reset and then begin the command again.  Because the RPC layer stores buffers the command, slave address and all parameters internally, it can retry the command without requiring the user code to do anything.  Also, since it is guaranteed that the command handler on the slave side was not run if the checksum failed, the master is free to reinciate the RPC without worrying that the command may not be idempotent.  

If the checksum fails from slave->master, the master will keep sending read indications in pairs of 2 and the slave will keep sending the same status and return value until the master stops sending reads, indicating that the checksum was correct.  Since the slave RPC layer buffers the return value from the command handler it ran as well as the return status, it can resend them freely without needing to consult usercode.  

Thus, once a MIB RPC has finished, we can be guaranteed that it completed correctly without a transient bus error.  

###Slave Return Value Transmission
Slaves always respond to read requests from the master with data according to the following protocol.  If the slave has received an even number of reads, it sends its return status.  If it has received an odd number of reads, then it sends its return value.  This allows the master to repeatedly query the return status and value until they are correctly received.  If the slave is asked to send a return value and it did not execute a command_handler that returned a value, then it will send 1 garbage byte which the master must ignore.  

```
Slave Read 1 - send return status (2 bytes)
Slave Read 2 - send return value or garbage (at least 1 byte)
...
Slave Read 2N+1 - send return status (2 bytes)
Slave Read 2N+2 - send return value or garbage (at least 1 byte)
```

##Slave Reset Procedure
A significant issue with the i2c protocol is that there is no resynchronizaton procedure.  If a slave misses one clock, it will be 1 bit out of sync and misinterpret all subsequent data.  An obvious solution is to have the slave key in on the i2c stop condition and reset itself to a known state when a stop is received.  However, the PIC24 and probable PIC12 do not interrupt when a stop is received :(.  As a workaround we define a bus-specific reset procedure for returning a slave to known state independent of the course of events that screwed it up.  

The reset procedure is based on the observation that in the normal MIB transaction flow, the master never sends a write after a read.  It writes a command, writes a parameter and then reads the return status and return value.  So, the slave can be sure that if it receives a write after having received a read in the same transaction that this is an exception and reset itself.  This is implemented by having the slave keep an internal counter of how many reads it has received in the transaction (num_reads).  If it receives a write and num_reads > 0, then it resets itself back to idle.  

Note that the master does not need to know how the slave got screwed up in order to reset it.  It can just send one read followed by one write and be sure that the slave will then be idle and ready to receive a command.

###Reset Procedure
```
Slave state unknown: num_reads = X >= 0
Master Sends Read (1 byte): num_reads = X+1 > 0
Master Sends Write (1 byte)
Slave resets itself 
```

##Master Logic Flowchart
Figure 2 below shows the complete MIB Master state machine including all error processing.  This shows error processing paths as well.

![](http://github.com/WellDone/MoMo-Firmware/wiki/Master-State-Flowchart.png)
Figure 2

##MIB FAQ
This section is to document why certain decisions were made.  This is not to say that they are the best option and should not be changed or revisited, but rather that we should understand the considerations that drove their choice so that a potential replacement doesn't fall afoul of the same problem.

###Why are the parameters all sent as a single packet?
This is to allow for easy command resending.  Currently the parameters are all composed into a packet in the mib_buffer and sent.  We know that they all fit into the mib_buffer since they are sent as a single packet which is limited in size to the size of the mib_buffer.  If there is a problem sending the parameters (arbitration lost or checksum error), we still have them all in the mib_buffer so we can resend them without needing usercode to provide them again.  This means the usercode can assume that the process always succeeds.

###Why is there a return status and a return value?
The return value is a typed parameter defined by the specific command being called, which the MIB layer knows nothing about.  However there needs to also be a way to indicate whether the command was successfully called or to signal that the parameters need to be resent, for example.  The return status serves this purpose.  Also, the slave needs to tell the master how many bytes are in in the return value (which can be a buffer so its unknown apriori).  So the return status sends a length to tell the master how many bytes to read in for the return value.  This also allows for void return values (the return status has a length of 0).