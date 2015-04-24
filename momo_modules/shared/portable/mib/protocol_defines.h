/*
 * The MIB Protocol v2.0
 * This file and protocol.h contain all of the information needed to speak 
 * and understand the MoMo remote procedure call protocol, known as the MIB 
 * (Module Interface Bus).
 * 
 * MIB calls are structured as fixed sized packets where a master sends a command
 * packet to a slave to initiate an RPC, the slave executes the function and then
 * the master reads a response packet containing the results.  
 *
 * Version 2.0 of the MIB Protocol adds the following features over the first version:
 * - Support for asynchronous RPC calls wheere the bus is not locked while the slave
 *   executes the callback.  Instead the slave makes a special master call to notify the
 *   original caller of the result of the RPC.
 * 
 * - Unified packet structure to make for simpler implementation on low-performance 8-bit
 *   devices. 
 *
 * - A fixed timeout for RPC calls so that the bus can never be locked forever if a slave
 *   receives an RPC call but never gets around to processing it.
 */

#ifndef __protocol_defines_h__
#define __protocol_defines_h__

//Fixed Sizes for the MIB Bus
#define kMIBBufferSize 				20
#define kMIBMessageSize				(kMIBBufferSize + 4 + 1)
#define kMIBMessageNoChecksumSize	(kMIBMessageSize - 1)

//Fixed Timeouts (in milliseconds, must be less than 256)
#define kMIBSlaveTimeout 	50

/*
 * Globally Known Status Codes
 * Each MIB call can return a 6-bit status code.  Some of these codes are application
 * defined.  The codes that have globally significant meaning are listed here.  They
 * live in a separate namespace from application defined status codes, so applications
 * may reuse the same numerical values.
 *
 * Status codes are 8 bits long with the 2 MSB having defined meanings:
 * bit 7:    whether the response has addition data beyond just the status code
 * bit 6:    whether the low 6 bits should be interpreted as a global status code or app defined
 * bits 0-5: status code
 */

#define kAppDefinedBit 	6
#define kHasDataBit		7

#define kMIBStatusCodeMask	0b00111111

#define make_mib_status(has_data, app_defined, code)	((((has_data & 1) << kHasDataBit)) | ((app_defined & 1) << kAppDefinedBit) | (code & 0b111111))

#define kModuleBusyCode				0x00
#define kChecksumMismatchCode 		0x01
#define kCommandNotFoundCode		0x02
#define kModuleNotPresentCode		0x3f

#define kNoErrorStatus				make_mib_status(0, 1, 0x00)
#define kModuleBusyStatus			make_mib_status(0, 0, kModuleBusyCode)
#define kChecksumMismatchStatus		make_mib_status(0, 0, kChecksumMismatchCode)
#define kCommandNotFoundStatus 		make_mib_status(0, 0, kCommandNotFoundCode)
#define kModuleNotPresentStatus		make_mib_status(1, 1, kModuleNotPresentCode)	//a missing module responds 0xFF on the bus

//Well-Known MIB Addresses
#define kMIBControllerAddress		8
#define kMIBUnenumeratedAddress		127

//Defined MIB Hardware Types
#define kMIBPic12lf1822					2
#define kMIBPic16lf1823					3
#define kMIBPic16lf1847					4
#define kMIBControllerFirmware			5
#define kMIBBackupControllerFirmware	6

//Defined MIB Protocol revisions
#define kMIBVersion1				1

//Special Command IDs
#define kExecutiveSpecialHighByte 	0x00

#endif
