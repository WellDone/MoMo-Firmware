/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */


// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
/** \file
 *  This header file includes the correct
 *  processor-specific include file. It also provides
 *  additional processor-specific information, such as
 *  the device ID, revision IDs, and the number of various
 *  peripherals.
 */

#ifndef _PIC24_CHIP_ALL_H_
#define _PIC24_CHIP_ALL_H_

// Include processor-specific header file
#if defined(__PIC24H__)
#include "p24Hxxxx.h"
#elif defined(__PIC24F__) || defined(__PIC24FK__)
#include "p24Fxxxx.h"
#elif defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#else
#error Unknown processor.
#endif

#ifdef _ISR
#undef _ISR
/// Override the standard Microchip ISR definition, since it produces the
/// following warning:
/// warning:  PSV model not specified for '_DefaultInterrupt';
///   assuming 'auto_psv' this may affect latency
/// This definition adds the auto_psv specifier, which produces
/// a bit of extra code but allow accessing constant data
/// (data stored in program memory).
#define _ISR __attribute__((interrupt)) __attribute__ ((auto_psv))
#endif

#ifdef _ISRFAST
#undef _ISRFAST
/// Unlike \ref _ISR, this definition does not allow the ISR to access
/// constant data (data stored in program memory). This saves some
/// clock cycles. Unlike the Microchip definition, it does NOT use
/// shadow registers as well.
#define _ISRFAST __attribute__((interrupt)) __attribute__ ((no_auto_psv))
#endif


/// Place the processor in sleep mode.
#define SLEEP()  Sleep()

/// Place the processor in idle mode.
#define IDLE()   Idle()

/// Clear the watch-dog timer.
#define CLRWDT() ClrWdt()

/// Insert a one-cycle nop instruction.
#define NOP() Nop()

/// Enable the watchdog timer.
#define ENABLEWDT() (_SWDTEN = 1)

/// Disable the watchdog timer.
#define DISABLEWDT() (_SWDTEN = 0)

/// \name Peripheral identification
//@{
/** Define the number of UART modules supported by this chip.
 *  NOTE: this only works when testing \#defines, not
 *  testing variable. So, don't test UxSTA; instead,
 *  test _UxRXIF which is uniquely defined for each
 *  UART. (Idea taken from the C30 peripheral library,
 *  in uart.h.)
 */
#if   defined(_U4RXIF)
#define NUM_UART_MODS 4
#elif defined(_U3RXIF)
#define NUM_UART_MODS 3
#elif defined(_U2RXIF)
#define NUM_UART_MODS 2
#elif defined(_U1RXIF) || defined(__DOXYGEN__)
#define NUM_UART_MODS 1
#else
#error UxRXIF flag not defined!
#endif

/** Define the number of I2C modules supported by this chip.
 *  See \ref NUM_UART_MODS for details on this methodology.
 */
#if   defined(_SI2C2IF)
#define NUM_I2C_MODS 2
#elif defined(_SI2C1IF) || defined(__DOXYGEN__)
#define NUM_I2C_MODS 1
#else
#error SI2CxIF flag not defined!
#endif

/** Define the number of SPI modules supported by this chip.
 *  See \ref NUM_UART_MODS for details on this methodology.
 */
#if   defined(_SPI2IF)
#define NUM_SPI_MODS 2
#elif defined(_SPI1IF) || defined(__DOXYGEN__)
#define NUM_SPI_MODS 1
#else
#error SPIxIF flag not defined!
#endif

/** Define the number of ECAN modules supported by this chip.
 *  See \ref NUM_UART_MODS for details on this methodology.
 */
#if   defined(_C2IF)
#define NUM_ECAN_MODS 2
#elif defined(_C1IF) || defined(__DOXYGEN__)
#define NUM_ECAN_MODS 1
#else
#define NUM_ECAN_MODS 0
#endif
//@}



/** \name Chip identification
 *  These routines assist in identifying the device and revision
 *  of the PIC in use. See checkDeviceAndRevision() for a usage
 *  example.
 */
//@{
/** The location in program memory of the device ID.
 *  Revision information is found in the Silicon Errata
 *  sheet for the device. The Device ID is contained
 * in the datasheet for the target device.
 */
#define DEV_ID_LOCATION 0xFF0000
/** The location in program memory of the revision. */
#define REVISION_LOCATION 0xFF0002


#if defined(__PIC24HJ12GP201__) || defined(__DOXYGEN__)
/** The device ID for this chip, located in program memory
 *  at \ref DEV_ID_LOCATION.
 */
#define DEV_ID 0x00080a
/** A string to identify this chip, printed by
 *  checkDeviceAndRevision().
 */
#define DEV_ID_STR "PIC24HJ12GP201"
/** A revision for this device, located in program memory
 *  at \ref REVISION_LOCATION. Additional revisions
 *  can be defined as EXPECTED_REVISIONn, where n = 1..5.
 */
#define EXPECTED_REVISION1 0x003001
/** A string to accompany this revision, printed by
 *  checkDeviceAndRevision(). Additional revision strings
 *  can be defined as EXPECTED_REVISIONn, where n = 1..5.
 */
#define EXPECTED_REVISION1_STR "A2"

#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003003
#define EXPECTED_REVISION3_STR "A4"
#endif

#ifdef __PIC24HJ12GP202__
#define DEV_ID 0x00080B
#define DEV_ID_STR "PIC24HJ12GP202"
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003003
#define EXPECTED_REVISION3_STR "A4"
#endif



/*  PIC24H32GP202/204,PIC24HJ16GP304  */

#ifdef __PIC24HJ16GP304__
#define DEV_ID 0x000F17
#define DEV_ID_STR "PIC24HJ16GP304"
#endif

#ifdef __PIC24HJ32GP202__
#define DEV_ID 0x000F1D
#define DEV_ID_STR "PIC24HJ32GP202"
#endif

#ifdef __PIC24HJ32GP204__
#define DEV_ID 0x000F1F
#define DEV_ID_STR "PIC24HJ32GP204"
#endif

#if (defined(__PIC24HJ16GP304__) || defined(__PIC24HJ32GP202__) || defined(__PIC24HJ32GP204__))
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003004
#define EXPECTED_REVISION3_STR "A4"
#define EXPECTED_REVISION4 0x003005
#define EXPECTED_REVISION4_STR "A5"
#define EXPECTED_REVISION5 0x003006
#define EXPECTED_REVISION5_STR "A6"
#endif

/*  PIC24H32GP302/304, PIC24HJGPX02/X04, PIC24HJ128GPX02/X04 */

#ifdef __PIC24HJ32GP302__
#define DEV_ID 0x000645
#define DEV_ID_STR "PIC24HJ32GP302"
#endif

#ifdef __PIC24HJ32GP304__
#define DEV_ID 0x000647
#define DEV_ID_STR "PIC24HJ32GP304"
#endif

#ifdef __PIC24HJ64GP202__
#define DEV_ID 0x000655
#define DEV_ID_STR "PIC24HJ64GP202"
#endif

#ifdef __PIC24HJ64GP204__
#define DEV_ID 0x000657
#define DEV_ID_STR "PIC24HJ64GP204"
#endif

#ifdef __PIC24HJ64GP502__
#define DEV_ID 0x000675
#define DEV_ID_STR "PIC24HJ64GP502"
#endif

#ifdef __PIC24HJ64GP504__
#define DEV_ID 0x000677
#define DEV_ID_STR "PIC24HJ64GP504"
#endif

#ifdef __PIC24HJ128GP202__
#define DEV_ID 0x000665
#define DEV_ID_STR "PIC24HJ128GP202"
#endif


#ifdef __PIC24HJ128GP204__
#define DEV_ID 0x000667
#define DEV_ID_STR "PIC24HJ128GP204"
#endif

#ifdef __PIC24HJ128GP502__
#define DEV_ID 0x00067d
#define DEV_ID_STR "PIC24HJ128GP502"
#endif

#ifdef __PIC24HJ128GP504__
#define DEV_ID 0x00067f
#define DEV_ID_STR "PIC24HJ128GP504"
#endif

#if (defined(__PIC24HJ128GP504__) || defined(__PIC24HJ128GP502__)\
|| defined(__PIC24HJ128GP204__) ||   defined(__PIC24HJ128GP202__)\
|| defined(__PIC24HJ64GP504__) ||   defined(__PIC24HJ64GP502__)\
|| defined(__PIC24HJ64GP204__) ||   defined(__PIC24HJ64GP202__)\
|| defined(__PIC24HJ32GP304__) ||   defined(__PIC24HJ32GP302__))
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A1"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A2"
#define EXPECTED_REVISION3 0x003003
#define EXPECTED_REVISION3_STR "A3"
#endif


/*  PIC24HJXXXGPX06/X08/X10 */

#if (defined(__PIC24HJ64GP206__) || defined(__PIC24HJ64GP206A__))
#define DEV_ID 0x000041
#define DEV_ID_STR "PIC24HJ64GP206"
#endif

#if (defined(__PIC24HJ64GP210__) || defined(__PIC24HJ64GP210A__))
#define DEV_ID 0x000047
#define DEV_ID_STR "PIC24HJ64GP210"
#endif

#if (defined(__PIC24HJ64GP506__) || defined(__PIC24HJ64GP506A__))
#define DEV_ID 0x000049
#define DEV_ID_STR "PIC24HJ64GP506"
#endif

#if (defined(__PIC24HJ64GP510__) || defined(__PIC24HJ64GP510A__))
#define DEV_ID 0x00004B
#define DEV_ID_STR "PIC24HJ64GP510"
#endif

#if (defined(__PIC24HJ128GP206__) || defined(__PIC24HJ128GP206A__))
#define DEV_ID 0x00005D
#define DEV_ID_STR "PIC24HJ128GP206"
#endif

#if (defined(__PIC24HJ128GP210__) || defined(__PIC24HJ128GP210A__))
#define DEV_ID 0x00005F
#define DEV_ID_STR "PIC24HJ128GP210"
#endif

#if (defined(__PIC24HJ128GP306__) || defined(__PIC24HJ128GP306A__))
#define DEV_ID 0x000065
#define DEV_ID_STR "PIC24HJ128GP306"
#endif

#if (defined(__PIC24HJ128GP310__) || defined(__PIC24HJ128GP310A__))
#define DEV_ID 0x000067
#define DEV_ID_STR "PIC24HJ128GP310"
#endif


#if defined(__PIC24HJ128GP506__) || defined(__PIC24HJ128GP506A__)
#define DEV_ID 0x000061
#define DEV_ID_STR "PIC24HJ128GP506/506A"
#endif

#if (defined(__PIC24HJ128GP510__) || defined(__PIC24HJ128GP510A__))
#define DEV_ID 0x000063
#define DEV_ID_STR "PIC24HJ128GP510"
#endif

#if (defined(__PIC24HJ256GP206__) )
#define DEV_ID 0x000071
#define DEV_ID_STR "PIC24HJ256GP206"
#endif

#ifdef __PIC24HJ256GP210__
#define DEV_ID 0x000073
#define DEV_ID_STR "PIC24HJ256GP210"
#endif


#ifdef __PIC24HJ256GP610__
#define DEV_ID 0x00007B
#define DEV_ID_STR "PIC24HJ256GP610"
#endif

#if (defined(__PIC24HJ64GP206__) || defined(__PIC24HJ64GP210__) || defined(__PIC24HJ64GP506__) || defined(__PIC24HJ64GP510__)\
|| defined(__PIC24HJ128GP206__) || defined(__PIC24HJ128GP210__) || defined(__PIC24HJ128GP306__)\
|| defined(__PIC24HJ128GP310__) || defined(__PIC24HJ128GP506__)|| defined(__PIC24HJ128GP510__)\
|| defined(__PIC24HJ256GP206__) || defined(__PIC24HJ256GP210__)|| defined(__PIC24HJ256GP610__))

#define EXPECTED_REVISION1 0x003002
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003004
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003040
#define EXPECTED_REVISION3_STR "A4"
#endif

#if (defined(__PIC24HJ64GP206A__) || defined(__PIC24HJ64GP210A__) || defined(__PIC24HJ64GP506A__) || defined(__PIC24HJ64GP510A__)\
|| defined(__PIC24HJ128GP206A__) || defined(__PIC24HJ128GP210A__) || defined(__PIC24HJ128GP306A__)\
|| defined(__PIC24HJ128GP310A__) || defined(__PIC24HJ128GP506A__)|| defined(__PIC24HJ128GP510A__)\
|| defined(__PIC24HJ256GP206A__) || defined(__PIC24HJ256GP210A__)|| defined(__PIC24HJ256GP610A__))
#define EXPECTED_REVISION1 0x003009
#define EXPECTED_REVISION1_STR "A3"
#define EXPECTED_REVISION2 0x00300A
#define EXPECTED_REVISION2_STR "A4"
#define EXPECTED_REVISION3 0x00300B
#define EXPECTED_REVISION3_STR "A5"
#endif

//24F Family

/* PIC24FJ128GA010 Family  */

#ifdef __PIC24FJ128GA010__
#define DEV_ID 0x00040D
#define DEV_ID_STR "PIC24FJ128GA010"
#endif

#ifdef __PIC24FJ96GA010__
#define DEV_ID 0x00040C
#define DEV_ID_STR "PIC24FJ96GA010"
#endif

#ifdef __PIC24FJ64GA010__
#define DEV_ID 0x00040B
#define DEV_ID_STR "PIC24FJ64GA010"
#endif

#ifdef __PIC24FJ128GA008__
#define DEV_ID 0x00040A
#define DEV_ID_STR "PIC24FJ128GA008"
#endif

#ifdef __PIC24FJ96GA008__
#define DEV_ID 0x000409
#define DEV_ID_STR "PIC24FJ96GA008"
#endif

#ifdef __PIC24FJ64GA008__
#define DEV_ID 0x000408
#define DEV_ID_STR "PIC24FJ64GA008"
#endif

#ifdef __PIC24FJ128GA006__
#define DEV_ID 0x000407
#define DEV_ID_STR "PIC24FJ128GA006"
#endif

#ifdef __PIC24FJ96GA006__
#define DEV_ID 0x000409
#define DEV_ID_STR "PIC24FJ96GA006"
#endif

#ifdef __PIC24FJ64GA006__
#define DEV_ID 0x000408
#define DEV_ID_STR "PIC24FJ64GA006"
#endif

#if (defined(__PIC24FJ128GA010__) || defined(__PIC24FJ96GA010__) || defined(__PIC24FJ64GA010__)\
|| defined(__PIC24FJ128GA008__) || defined(__PIC24FJ96GA008__) || defined(__PIC24FJ64GA008__)\
|| defined(__PIC24FJ128GA006__) || defined(__PIC24FJ96GA006__) || defined(__PIC24FJ64GA006__))
#define EXPECTED_REVISION1 0x003002
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003003
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003007
#define EXPECTED_REVISION3_STR "A4"
#define EXPECTED_REVISION4 0x003043
#define EXPECTED_REVISION4_STR "C1"
#define EXPECTED_REVISION5 0x003044
#define EXPECTED_REVISION5_STR "C2"
#endif



//PIC24FJ64GA104 Family
#ifdef __PIC24FJ32GA102__
#define DEV_ID 0x004202
#define DEV_ID_STR "PIC24FJ32GA102"
#endif
#ifdef __PIC24FJ32GA104__
#define DEV_ID 0x00420A
#define DEV_ID_STR "PIC24FJ32GA104"
#endif
#ifdef __PIC24FJ64GA102__
#define DEV_ID 0x004206
#define DEV_ID_STR "PIC24FJ64GA102"
#endif
#ifdef __PIC24FJ64GA104__
#define DEV_ID 0x00420E
#define DEV_ID_STR "PIC24FJ64GA104"
#endif


#if (defined(__PIC24FJ32GA102__) || defined(__PIC24FJ32GA104__)\
|| defined(__PIC24FJ64GA102__) || defined(__PIC24FJ64GA104__))
#define EXPECTED_REVISION1 0x000002
#define EXPECTED_REVISION1_STR "A2"
#endif


//PIC24FJXXGA002 (28 pin),PIC24FJXXGA004 (44 pin)
#ifdef __PIC24FJ16GA002__
#define DEV_ID 0x000444
#define DEV_ID_STR "PIC24FJ16GA002"
#endif

#ifdef __PIC24FJ32GA002__
#define DEV_ID 0x000445
#define DEV_ID_STR "PIC24FJ32GA002"
#endif

#ifdef __PIC24FJ48GA002__
#define DEV_ID 0x000446
#define DEV_ID_STR "PIC24FJ48GA002"
#endif

#ifdef __PIC24FJ64GA002__
#define DEV_ID 0x000447
#define DEV_ID_STR "PIC24FJ64GA002"
#endif

#ifdef __PIC24FJ16GA004__
#define DEV_ID 0x00044C
#define DEV_ID_STR "PIC24FJ16GA004"
#endif

#ifdef __PIC24FJ32GA004__
#define DEV_ID 0x00044D
#define DEV_ID_STR "PIC24FJ32GA004"
#endif

#ifdef __PIC24FJ48GA004__
#define DEV_ID 0x00044E
#define DEV_ID_STR "PIC24FJ48GA004"
#endif

#ifdef __PIC24FJ64GA004__
#define DEV_ID 0x00044F
#define DEV_ID_STR "PIC24FJ64GA004"
#endif

#if (defined(__PIC24FJ64GA004__) || defined(__PIC24FJ48GA004__) || defined(__PIC24FJ32GA004__) || defined(__PIC24FJ16GA004__)\
|| defined(__PIC24FJ64GA002__) || defined(__PIC24FJ48GA002__) || defined(__PIC24FJ32GA002__) || defined(__PIC24FJ16GA002__))

#define EXPECTED_REVISION1 0x003003
#define EXPECTED_REVISION1_STR "A3/A4"
#define EXPECTED_REVISION2 0x003042
#define EXPECTED_REVISION2_STR "B4"
#define EXPECTED_REVISION3 0x003043
#define EXPECTED_REVISION3_STR "B5"
#define EXPECTED_REVISION4 0x003046
#define EXPECTED_REVISION4_STR "B8"
#endif


/* GA110, GA108,GA106 Family */


#ifdef __PIC24FJ256GA110__
#define DEV_ID 0x00101E
#define DEV_ID_STR "PIC24FJ256GA110"
#endif

#ifdef __PIC24FJ192GA110__
#define DEV_ID 0x001016
#define DEV_ID_STR "PIC24FJ192GA110"
#endif

#ifdef __PIC24FJ128GA110__
#define DEV_ID 0x00100E
#define DEV_ID_STR "PIC24FJ128GA110"
#endif

#ifdef __PIC24FJ256GA108__
#define DEV_ID 0x00101A
#define DEV_ID_STR "PIC24FJ256GA108"
#endif

#ifdef __PIC24FJ192GA108__
#define DEV_ID 0x001012
#define DEV_ID_STR "PIC24FJ192GA108"
#endif

#ifdef __PIC24FJ128GA108__
#define DEV_ID 0x00100A
#define DEV_ID_STR "PIC24FJ128GA108"
#endif

#ifdef __PIC24FJ256GA106__
#define DEV_ID 0x001018
#define DEV_ID_STR "PIC24FJ256GA106"
#endif

#ifdef __PIC24FJ192GA106__
#define DEV_ID 0x001010
#define DEV_ID_STR "PIC24FJ192GA106"
#endif

#ifdef __PIC24FJ128GA106__
#define DEV_ID 0x001008
#define DEV_ID_STR "PIC24FJ128GA106"
#endif

#if (defined(__PIC24FJ256GA110__) || defined(__PIC24FJ192GA110__) || defined(__PIC24FJ128GA110__)\
|| defined(__PIC24FJ256GA108__) || defined(__PIC24FJ192GA108__) || defined(__PIC24FJ128GA108__)\
|| defined(__PIC24FJ256GA106__) || defined(__PIC24FJ192GA106__) || defined(__PIC24FJ128GA106__))

#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A3"
#define EXPECTED_REVISION2 0x003003
#define EXPECTED_REVISION2_STR "A5"
#endif

/***** PIC24F K ****/

#ifdef __PIC24F04KA200__
#define DEV_ID 0x000D02
#define DEV_ID_STR "PIC24F04KA200"
#endif

#ifdef __PIC24F04KA201__
#define DEV_ID 0x000D00
#define DEV_ID_STR "PIC24F04KA201"
#endif

#if (defined(__PIC24F04KA200__) || defined(__PIC24F04KA201__) )
#define EXPECTED_REVISION1 0x000001
#define EXPECTED_REVISION1_STR "A1"
#endif


#ifdef __PIC24F08KA101__
#define DEV_ID 0x000D08
#define DEV_ID_STR "PIC24F08KA101"
#endif

#ifdef __PIC24F08KA102__
#define DEV_ID 0x000D0A
#define DEV_ID_STR "PIC24F08KA102"
#endif

#ifdef __PIC24F16KA101__
#define DEV_ID 0x000D01
#define DEV_ID_STR "PIC24F16KA101"
#endif

#ifdef __PIC24F16KA102__
#define DEV_ID 0x000D03
#define DEV_ID_STR "PIC24F16KA102"
#endif

#if (defined(__PIC24F08KA101__) || defined(__PIC24F08KA102__) || defined(__PIC24F16KA101__)\
|| defined(__PIC24F16KA102__))

#define EXPECTED_REVISION1 0x000005
#define EXPECTED_REVISION1_STR "A5"
#define EXPECTED_REVISION2 0x000006
#define EXPECTED_REVISION2_STR "A6"
#endif



/***** PIC24F GB ****/

#ifdef __PIC24FJ256GB110__
#define DEV_ID 0x00101F
#define DEV_ID_STR "PIC24FJ256GB110"
#endif

#ifdef __PIC24FJ192GB110__
#define DEV_ID 0x001017
#define DEV_ID_STR "PIC24FJ192GB110"
#endif

#ifdef __PIC24FJ128GB110__
#define DEV_ID 0x00100F
#define DEV_ID_STR "PIC24FJ128GB110"
#endif

#ifdef __PIC24FJ64GB110__
#define DEV_ID 0x001007
#define DEV_ID_STR "PIC24FJ64GB110"
#endif

#ifdef __PIC24FJ256GB108__
#define DEV_ID 0x00101B
#define DEV_ID_STR "PIC24FJ256GB108"
#endif

#ifdef __PIC24FJ192GB108__
#define DEV_ID 0x001013
#define DEV_ID_STR "PIC24FJ192GB108"
#endif

#ifdef __PIC24FJ128GB108__
#define DEV_ID 0x00100B
#define DEV_ID_STR "PIC24FJ128GB108"
#endif

#ifdef __PIC24FJ64GB108__
#define DEV_ID 0x001003
#define DEV_ID_STR "PIC24FJ64GB108"
#endif

#ifdef __PIC24FJ256GB106__
#define DEV_ID 0x001018
#define DEV_ID_STR "PIC24FJ256GB106"
#define EXPECTED_REVISION1 0x000001
#define EXPECTED_REVISION1_STR "A3"
#endif

#ifdef __PIC24FJ192GB106__
#define DEV_ID 0x001010
#define DEV_ID_STR "PIC24FJ192GB106"
#endif

#ifdef __PIC24FJ128GB106__
#define DEV_ID 0x001008
#define DEV_ID_STR "PIC24FJ128GB106"
#endif

#ifdef __PIC24FJ64GB106__
#define DEV_ID 0x001001
#define DEV_ID_STR "PIC24FJ64GB106"
#endif

#ifdef __PIC24FJ32GB002__
#define DEV_ID 0x004203
#define DEV_ID_STR "PIC24FJ32GB002"
#endif

#ifdef __PIC24FJ32GB004__
#define DEV_ID 0x00420B
#define DEV_ID_STR "PIC24FJ32GB004"
#endif

#ifdef __PIC24FJ64GB002__
#define DEV_ID 0x004207
#define DEV_ID_STR "PIC24FJ64GB002"
#endif

#ifdef __PIC24FJ64GB004__
#define DEV_ID 0x00420F
#define DEV_ID_STR "PIC24FJ64GB004"
#endif

#if (defined(__PIC24FJ32GB002__) || defined(__PIC24FJ32GB004__) || defined(__PIC24FJ64GB002__) || defined(__PIC24FJ64GB004__))
#define EXPECTED_REVISION1 0x000002
#define EXPECTED_REVISION1_STR "A2"
#endif


#if (defined(__PIC24FJ256GB110__) || defined(__PIC24FJ192GB110__) || defined(__PIC24FJ128GB110__) || defined(__PIC24FJ64GB110__)\
|| defined(__PIC24FJ256GB108__) || defined(__PIC24FJ192GB108__) || defined(__PIC24FJ128GB108__) || defined(__PIC24FJ64GB108__)\
|| defined(__PIC24FJ256GB106__) || defined(__PIC24FJ192GB106__) || defined(__PIC24FJ128GB106__) || defined(__PIC24FJ64GB106__))

#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A3"
#define EXPECTED_REVISION2 0x003003
#define EXPECTED_REVISION2_STR "A5"
#endif

//dsPIC33 CPUS

//dsPIC33FJ12GP201, dsPIC33FJ12GP202
#ifdef __dsPIC33FJ12GP201__
#define DEV_ID 0x000802
#define DEV_ID_STR "dsPIC33FJ12GP201"
#endif

#ifdef __dsPIC33FJ12GP202__
#define DEV_ID 0x000803
#define DEV_ID_STR "dsPIC33FJ12GP202"
#endif

#if (defined(__dsPIC33FJ12GP201__) || defined(__dsPIC33FJ12GP202__))
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003004
#define EXPECTED_REVISION3_STR "A4"
#endif

//dsPIC33FJ32GP202/04,dsPIC33FJ16GP304

#ifdef __dsPIC33FJ32GP202__
#define DEV_ID 0x000F0D
#define DEV_ID_STR "dsPIC24FJ32GP202"
#endif

#ifdef __dsPIC33FJ32GP204__
#define DEV_ID 0x000F0F
#define DEV_ID_STR "dsPIC33FJ32GP204"
#endif

#ifdef __dsPIC33FJ16GP304__
#define DEV_ID 0x000F07
#define DEV_ID_STR "dsPIC33FJ16GP304"
#endif

#if (defined(__dsPIC33FJ32GP202__) || defined(__dsPIC33FJ32GP204___) || defined(__dsPIC33FJ16GP304__))
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A2"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A3"
#define EXPECTED_REVISION3 0x003004
#define EXPECTED_REVISION3_STR "A4"
#endif

//dsPIC33FJ32GP302/304,dsPIC33FJ64GPX02/X04,dsPIC33FJ128GPX02,X04

#ifdef __dsPIC33FJ32GP302__
#define DEV_ID 0x000605
#define DEV_ID_STR "dsPIC33FJ32GP302"
#endif

#ifdef __dsPIC33FJ32GP304__
#define DEV_ID 0x000607
#define DEV_ID_STR "dsPIC33FJ32GP304"
#endif

#ifdef __dsPIC33FJ64GP202__
#define DEV_ID 0x000615
#define DEV_ID_STR "dsPIC33FJ64GP202"
#endif

#ifdef __dsPIC33FJ64GP204__
#define DEV_ID 0x000617
#define DEV_ID_STR "dsPIC33FJ64GP204"
#endif

#ifdef __dsPIC33FJ64GP802__
#define DEV_ID 0x00061d
#define DEV_ID_STR "dsPIC33FJ64GP802"
#endif

#ifdef __dsPIC33FJ64GP804__
#define DEV_ID 0x00061f
#define DEV_ID_STR "dsPIC33FJ64GP804"
#endif

#ifdef __dsPIC33FJ128GP202__
#define DEV_ID 0x000625
#define DEV_ID_STR "dsPIC33FJ128GP202"
#endif


#ifdef __dsPIC33FJ128GP204__
#define DEV_ID 0x000627
#define DEV_ID_STR "dsPIC33FJ128GP204"
#endif

#ifdef __dsPIC33FJ128GP802__
#define DEV_ID 0x00062d
#define DEV_ID_STR "dsPIC33FJ128GP802"
#endif

#ifdef __dsPIC33FJ128GP804__
#define DEV_ID 0x00062f
#define DEV_ID_STR "dsPIC33FJ128GP804"
#endif

#if (defined(__dsPIC33FJ128GP804__) || defined(__dsPIC33FJ128GP802__)\
|| defined(__dsPIC33FJ128GP204__) ||   defined(__dsPIC33FJ128GP202__)\
|| defined(__dsPIC33FJ64GP804__) ||   defined(__dsPIC33FJ64GP802__)\
|| defined(__dsPIC33FJ64GP204__) ||   defined(__dsPIC33FJ64GP202__)\
|| defined(__dsPIC33FJ32GP304__) ||   defined(__dsPIC33FJ32GP302__))
#define EXPECTED_REVISION1 0x003001
#define EXPECTED_REVISION1_STR "A1"
#define EXPECTED_REVISION2 0x003002
#define EXPECTED_REVISION2_STR "A2"
#define EXPECTED_REVISION3 0x003003
#define EXPECTED_REVISION3_STR "A3"
#endif




/* Use these default device ID and revision values
   if you are trying to discover the ID/Revision by
   executing code on your target device and watching
   was is printed by the printResetCause() function.
*/
#ifndef DEV_ID
#define DEV_ID 0
#define DEV_ID_STR "unknown"
#define EXPECTED_REVISION1 0
#define EXPECTED_REVISION1_STR "unknown"
#warning *********************************************
#warning * Unknown chip type! Define a device ID and *
#warning * revision ID in the lines above for this   *
#warning * chip to remove this warning message.      *
#warning * These values can be found by              *
#warning * executing the checkDeviceAndRevision()    *
#warning * function, which is typically called after *
#warning * reset via calling printResetCause().      *
#warning * This check allows you to ensure that the  *
#warning * programmed hex file is running on the     *
#warning * same device type it was compiled for.     *
#warning * This is only a warning.                   *
#warning *********************************************
#endif

//@}

#endif // #ifndef  _PIC24_CHIP_ALL_H_
