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
 *  This header file includes the all the pic24_*.h files as detailed
 *  in the \ref index and also includes the necessary
 *  processor-specific include file.
 */

#ifndef _PIC24_ALL_H_
#define _PIC24_ALL_H_

// Include processor-specific header file
#include "pic24_chip.h"

// Include user-configurable options
#include "pic24_libconfig.h"

// Include PIC24 support library headers
#include "stdint.h"
#include "pic24_unittest.h"
#include "pic24_clockfreq.h"
#include "pic24_delay.h"
#include "pic24_ports.h"

#ifndef BUILT_ON_ESOS
#include "pic24_uart.h"
#include "pic24_util.h"
#include "pic24_serial.h"
#endif

#include "pic24_timer.h"
#include "pic24_i2c.h"
#include "pic24_spi.h"
#include "pic24_adc.h"
#include "pic24_dma.h"
#include "pic24_ecan.h"
#include "pic24_flash.h"
#include "pic24_comparator.h"

#endif // #ifndef  _PIC24_ALL_H_
