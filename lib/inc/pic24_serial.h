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
 *  This file defines routines to send and receive data
 *  over the serial port. All hardware-specific routines
 *  are contained elsewhere.
 *  These are lightweight character I/O functions and are
 *  intended for use when \em printf overhead is too costly.
 */

#ifndef _PIC24_SERIAL_H_
#define _PIC24_SERIAL_H_

#include "pic24_libconfig.h"
#include "stdint.h"
#include "pic24_chip.h"


// Consistency check
#if (DEFAULT_UART > NUM_UART_MODS) || (DEFAULT_UART < 1)
#error Invalid choice of DEFAULT_UART
#endif



uint8_t inChar(void);  //blocking single character input
void outString(const char* psz_s);
uint16_t inString (char *psz_buff, int16_t u16_maxCount);
uint16_t inStringEcho (char *psz_buff, int16_t u16_maxCount);
void outChar(uint8_t u8_c);
void outUint8NoLeader (uint8_t u8_x);
void outUint8 (uint8_t u8_x);
void outUint16(uint16_t u16_x);
void outUint32(uint32_t u32_x);
void outUint8Decimal(uint8_t u8_x);
void outUint16Decimal(uint16_t u16_x);
uint8_t isCharReady(void); //determine if character is available to be read

uint8_t inCharEcho(void);
void configDefaultUART(uint32_t u32_baudRate);

#endif
