/*bootloader.h
 *
 * An I2C command processor with integrated reflashing capabilities.  The command processing is integrated here
 * so that the i2c bootloader is self-contained and can safely reflash the rest of memory.  Commands that are 
 * not handled natively in the bootloader code (i.e. all commands except the ones pertaining to reflashing) are 
 * passed to the application code for processing
 */

#ifndef __bootloader_h__
#define __bootloader_h__

#define _BOOTLOADER_CODE 	__section(".bootloader")

#define kPic24FirmwareSubsection1 0x1000
#define kPic24FirmwareSubsection2 0x2000
#define kPic24FirmwareSubsection3 0x3000
#define kPic24FirmwareSubsection4 0x4000
#define kPic24FirmwareSubsection5 0x5000


#endif