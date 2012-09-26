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


#ifndef _PIC24_UART1_H_
#define _PIC24_UART1_H_

#include "pic24_libconfig.h"
#include "stdint.h"
#include "pic24_chip.h"
#include "pic24_util.h"

// Only include if this UART module exists.
#if (NUM_UART_MODS >= 1)

// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
 *  This file contains routines which configure and
 *  use the UARTs on the PIC. See \ref pic24_serial.h
 *  for higher-level routines, which should typically
 *  be called by the user rather than these routines.
 */


/** Determine if a character is available in the UART's
 *  receive buffer.
 *  \return True (1) if character is available, 0 if not.
 */
#define IS_CHAR_READY_UART1() U1STAbits.URXDA

/** Determine if a the transmit buffer is full.
 *  \return True (1) if the transmit buffer if full,
 *          false (0) if not.
 */
#define IS_TRANSMIT_BUFFER_FULL_UART1() U1STAbits.UTXBF

/** Determines if all characters placed in the UART have been sent.
 *  Returns 1 if the last transmission has completed, or 0 if a transmission
 *  is in progress or queued in the transmit buffer.
 *  \return True (1) if the last transmission has completed, 0 if not.
 */
#define IS_TRANSMIT_COMPLETE_UART1() U1STAbits.TRMT

/** Waits until all characters placed in the UART have been sent. */
inline static void WAIT_UNTIL_TRANSMIT_COMPLETE_UART1() {
  while (!IS_TRANSMIT_COMPLETE_UART1())
    doHeartbeat();
}

void outChar1(uint8_t u8_c);
uint8_t inChar1(void);
void configUART1(uint32_t u32_baudRate);
uint8_t isCharReady1(void);
void checkRxErrorUART1(void); //check for UART RX error

#endif // #if (NUM_UART_MODS >= 1)
#endif // #ifndef _PIC24_UART1_H_








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


#ifndef _PIC24_UART2_H_
#define _PIC24_UART2_H_

#include "pic24_libconfig.h"
#include "stdint.h"
#include "pic24_chip.h"
#include "pic24_util.h"

// Only include if this UART module exists.
#if (NUM_UART_MODS >= 2)

// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
 *  This file contains routines which configure and
 *  use the UARTs on the PIC. See \ref pic24_serial.h
 *  for higher-level routines, which should typically
 *  be called by the user rather than these routines.
 */


/** Determine if a character is available in the UART's
 *  receive buffer.
 *  \return True (1) if character is available, 0 if not.
 */
#define IS_CHAR_READY_UART2() U2STAbits.URXDA

/** Determine if a the transmit buffer is full.
 *  \return True (1) if the transmit buffer if full,
 *          false (0) if not.
 */
#define IS_TRANSMIT_BUFFER_FULL_UART2() U2STAbits.UTXBF

/** Determines if all characters placed in the UART have been sent.
 *  Returns 1 if the last transmission has completed, or 0 if a transmission
 *  is in progress or queued in the transmit buffer.
 *  \return True (1) if the last transmission has completed, 0 if not.
 */
#define IS_TRANSMIT_COMPLETE_UART2() U2STAbits.TRMT

/** Waits until all characters placed in the UART have been sent. */
inline static void WAIT_UNTIL_TRANSMIT_COMPLETE_UART2() {
  while (!IS_TRANSMIT_COMPLETE_UART2())
    doHeartbeat();
}

void outChar2(uint8_t u8_c);
uint8_t inChar2(void);
void configUART2(uint32_t u32_baudRate);
uint8_t isCharReady2(void);
void checkRxErrorUART2(void); //check for UART RX error

#endif // #if (NUM_UART_MODS >= 2)
#endif // #ifndef _PIC24_UART2_H_








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


#ifndef _PIC24_UART3_H_
#define _PIC24_UART3_H_

#include "pic24_libconfig.h"
#include "stdint.h"
#include "pic24_chip.h"
#include "pic24_util.h"

// Only include if this UART module exists.
#if (NUM_UART_MODS >= 3)

// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
 *  This file contains routines which configure and
 *  use the UARTs on the PIC. See \ref pic24_serial.h
 *  for higher-level routines, which should typically
 *  be called by the user rather than these routines.
 */


/** Determine if a character is available in the UART's
 *  receive buffer.
 *  \return True (1) if character is available, 0 if not.
 */
#define IS_CHAR_READY_UART3() U3STAbits.URXDA

/** Determine if a the transmit buffer is full.
 *  \return True (1) if the transmit buffer if full,
 *          false (0) if not.
 */
#define IS_TRANSMIT_BUFFER_FULL_UART3() U3STAbits.UTXBF

/** Determines if all characters placed in the UART have been sent.
 *  Returns 1 if the last transmission has completed, or 0 if a transmission
 *  is in progress or queued in the transmit buffer.
 *  \return True (1) if the last transmission has completed, 0 if not.
 */
#define IS_TRANSMIT_COMPLETE_UART3() U3STAbits.TRMT

/** Waits until all characters placed in the UART have been sent. */
inline static void WAIT_UNTIL_TRANSMIT_COMPLETE_UART3() {
  while (!IS_TRANSMIT_COMPLETE_UART3())
    doHeartbeat();
}

void outChar3(uint8_t u8_c);
uint8_t inChar3(void);
void configUART3(uint32_t u32_baudRate);
uint8_t isCharReady3(void);
void checkRxErrorUART3(void); //check for UART RX error

#endif // #if (NUM_UART_MODS >= 3)
#endif // #ifndef _PIC24_UART3_H_








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


#ifndef _PIC24_UART4_H_
#define _PIC24_UART4_H_

#include "pic24_libconfig.h"
#include "stdint.h"
#include "pic24_chip.h"
#include "pic24_util.h"

// Only include if this UART module exists.
#if (NUM_UART_MODS >= 4)

// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
 *  This file contains routines which configure and
 *  use the UARTs on the PIC. See \ref pic24_serial.h
 *  for higher-level routines, which should typically
 *  be called by the user rather than these routines.
 */


/** Determine if a character is available in the UART's
 *  receive buffer.
 *  \return True (1) if character is available, 0 if not.
 */
#define IS_CHAR_READY_UART4() U4STAbits.URXDA

/** Determine if a the transmit buffer is full.
 *  \return True (1) if the transmit buffer if full,
 *          false (0) if not.
 */
#define IS_TRANSMIT_BUFFER_FULL_UART4() U4STAbits.UTXBF

/** Determines if all characters placed in the UART have been sent.
 *  Returns 1 if the last transmission has completed, or 0 if a transmission
 *  is in progress or queued in the transmit buffer.
 *  \return True (1) if the last transmission has completed, 0 if not.
 */
#define IS_TRANSMIT_COMPLETE_UART4() U4STAbits.TRMT

/** Waits until all characters placed in the UART have been sent. */
inline static void WAIT_UNTIL_TRANSMIT_COMPLETE_UART4() {
  while (!IS_TRANSMIT_COMPLETE_UART4())
    doHeartbeat();
}

void outChar4(uint8_t u8_c);
uint8_t inChar4(void);
void configUART4(uint32_t u32_baudRate);
uint8_t isCharReady4(void);
void checkRxErrorUART4(void); //check for UART RX error

#endif // #if (NUM_UART_MODS >= 4)
#endif // #ifndef _PIC24_UART4_H_








