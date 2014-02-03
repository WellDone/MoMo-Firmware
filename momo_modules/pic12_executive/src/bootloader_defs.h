#ifndef __bootloader_defs_h
#define __bootloader_defs_h

/*
 * Define platform specific programming row sizes and other variables
 * based on platform data passed from the MIB build system.  
 *
 * PIC12 processors are programmed by row where the row is composed of N
 * words that are written to latches and then programmed at once.
 * N depends on the memory size of the processor and is usually a power of 2.
 * For the pic16f1823 and pic12f1822, N = 16, for the pic16lf1847, N = 32
 */

#define kBootloaderBufferSize 		(kFlashRowSize*2)
#define kBootloaderBufferLoc		0x20	//bootloader uses first 2N bytes of ram in bank0 to cache flash row

#define kMIBRequestWords			8
#define kMIBRequestSize				(kMIBRequestWords*2)
#define kNumMIBRequests				(kFlashRowSize/kMIBRequestWords)
#define kFinalPartialOffset			((kNumMIBRequests - 1)*kMIBRequestWords)

#define kNumFlashRows				(kFlashMemorySize / kFlashRowSize)
#define kMIBStructRow				((2048 / kFlashRowSize) - 1)

#if (kFlashRowSize != 16) && (kFlashRowSize != 32)
#error Currently only 16 byte and 32 byte flash memory rows are supported
#endif

//Sanity checks to make sure we can support this chip type
#if (kFlashMemorySize % kFlashRowSize) != 0
#error The flash memory size must be a multiple of the flash row size
#endif

#if (kBootloaderBufferSize % kMIBRequestSize) != 0
#error The bootloader buffer size must be a multiple of the MIB request size
#endif

#if (kBootloaderBufferSize > 64)
#error Cannot handler bootloader buffer sizes greater than 64 bytes yet.
#endif

#if (kNumFlashRows > 256)
#error Cannot handle a chip with more than 256 rows.
#endif

#endif