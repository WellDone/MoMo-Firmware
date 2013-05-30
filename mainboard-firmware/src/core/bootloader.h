/*bootloader.h
 *
 * An I2C command processor with integrated reflashing capabilities.  The command processing is integrated here
 * so that the i2c bootloader is self-contained and can safely reflash the rest of memory.  Commands that are 
 * not handled natively in the bootloader code (i.e. all commands except the ones pertaining to reflashing) are 
 * passed to the application code for processing
 */
#ifndef __bootloader_h__
#define __bootloader_h__

#include "bus.h"

#define _BOOTLOADER_CODE 	__section(".bootloader")

//Bootloader functions
void bootloader_process_command();

#endif