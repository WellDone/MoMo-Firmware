//bootloader.c

#include "bootloader.h"

extern volatile MIBState mib_state;

/* Logic for this routine.  
 *
 */

void __attribute__((interrupt,no_auto_psv,section(".bootloader"))) _SI2C1Interrupt()
{
	//If we just received a stop, end the command if there is one executing and 
	//go back into idle mode
	if (_P)
	{
		if (mib_state.receive_state == kExecutingCommandState)
		{
			
		}
		else
		{
			mib_state.receive_state = kIdleState;
			mib_state.last_error = kNoError;
		}

		i2c_release_clock();
		goto cleanup;
	}

	//If we've already received our command, this is just a command dependent byte
	//so pass it on to the command continuation
	if (mib_state.receive_state == kExecutingCommandState)
	{
		if (_RBF)
			mib_state.received_byte = I2C1RCV;

		taskloop_add(mib_state.command_continuation);
		//do not release clock.  This is the job of the command continuation
		goto cleanup;
	}

	//If we're in an error state, do nothing (only cleared with a stop condition)
	if (mib_state.receive_state == kErrorState)
	{
		i2c_release_clock();
		goto cleanup;
	}

	/*
	 * Logic for receiving and processing command packets
	 */

	//If we have not already fully received a command, we need to read in a command
	//packet, decide if we support it, respond with an status code (could be success) 
	//and then enter the kExecutingCommandState


	//If we just received an address with a write bit, begin waiting for the command packet
	//Otherwise it could either be that we're supposed to respond to the previously received
	//command packet or it's a protocol error
	if (_D_NOT_A == 0)
	{
		//A write after an address (when not already excecuting a command) 
		//means we're getting a command packet
		//this is all we do here, we have to wait for the command bytes to clock in
		if (_R_NOT_W == 0)
		{
			mib_state.receive_state = kReceivingCommandState;
			mib_state.curr_cmd_byte = 0;
			//hardware automaticaly acks and releases clock for an address
		}
		else if (mib_state.receive_state == kProcessedCommandState)	
		{
			//We are supposed to report back if this was a valid command
			I2C1TRN = mib_state.last_error;
			i2c_release_clock();

			if (mib_state.last_error == kNoError)
				mib_state.receive_state = kExecutingCommandState;
		}
		else
		{
			//We have a protocol error (tried to read a byte from us when not supported)
			mib_state.receive_state = kErrorState;
			mib_state.last_error = kProtocolError;
			i2c_release_clock();
		}

		goto cleanup;
		
	}

	if (mib_state.receive_state == kReceivingCommandState && _RBF)
	{
		//We received another byte in the command packet
		mib_state.curr_cmd.bytes[mib_state.curr_cmd_byte++] = I2C1RCV;

		//Check if we've received a complete command
		if (mib_state.curr_cmd_byte == kMIBCommandLength)
		{
			//make sure the checksum is valid (2's complement of sum of the bytes)
			unsigned char check = mib_state.curr_cmd.sep.feature + mib_state.curr_cmd.sep.command;
			check = ~check + 1;

			if (check == mib_state.curr_cmd.sep.checksum)
			{
				//We've received a valid command packet (do not release clock until we process it)
				//Schedule a task to process it
				mib_state.receive_state = kReceivedCommandState;
				taskloop_add(bootloader_process_command);
			}
			else
			{
				//We've received a malformed command packet, set an error condition and jump straight
				//to kProcessedCommandState so we can report this error to the master
				mib_state.receive_state = kProcessedCommandState;
				mib_state.last_error = kMalformedCommandPacketError;
				i2c_release_clock();
			}
		}
	}
	else
	{
		//We have a protocol error since the above cases should be exhaustive of what's permitted
		mib_state.receive_state = kErrorState;
		mib_state.last_error = kProtocolError;
		i2c_release_clock();
	}

	//Clear the interrupt flag and return
	cleanup:
	_SI2C1IF = 0;
}

/*
 * bootloader_process_command
 * Take a valid command packet that has been received and decided if we support the command that
 * it specifies. Set the appropriate error condition and transition to the kProcessedCommandState
 */

void __attribute__((section(".bootloader"))) bootloader_process_command()
{
	mib_state.last_error = kUnsupportedFeatureError;
	mib_state.receive_state = kReceivedCommandState;
}