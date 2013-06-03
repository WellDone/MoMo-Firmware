//bootloader.c

#include "bootloader.h"

extern volatile MIBState mib_state;

/*
 * bootloader_process_command
 * Take a valid command packet that has been received and decided if we support the command that
 * it specifies. Set the appropriate error condition and transition to the kProcessedCommandState
 */

void __attribute__((section(".bootloader"))) bootloader_process_command()
{
	
}