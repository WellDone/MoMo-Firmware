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




#include "pic24_clockfreq.h"
#include "pic24_uart.h"
#include "pic24_ports.h"
#include "pic24_unittest.h"
#include "pic24_delay.h"

// Only include if this UART exists.
#if (NUM_UART_MODS >= 1)


// Documentation for this file. If the \file tag is not present,
// this file will not be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
*  UART support functions.
*  \par Interrupt-driven TX/RX
*  By default, the UART functions use polling for both RX and TX.
*  Define the macro UARTx_TX_INTERRUPT (i.e., UART1_TX_INTERRUPT) in your project file if you want interrupt-driven TX for the UARTx (i.e., UART1) module.
*  For interrupt-driven TX, macro UARTx_TX_FIFO_SIZE sets the TX software FIFO size (default 32), and UARTx_TX_INTERRUPT_PRIORITY  sets the priority (default 1).
* \par
*  Define the macro UARTx_RX_INTERRUPT (i.e., UART1_RX_INTERRUPT) in your project file if you want interrupt-driven RX for the UARTx (i.e., UART1) module.
*  For interrupt-driven RX, macro UARTx_RX_FIFO_SIZE sets the RX software FIFO size (default 32), and UARTx_RX_INTERRUPT_PRIORITY  sets the priority (default 1).
*/


/*********************************
 * Function private to this file *
 *********************************/






/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/
/**
* Check UART1 RX for error, call \em reportError() if error found.
*
*/
void checkRxErrorUART1(void) {
  uint8_t u8_c;
//check for errors, reset if detected.
  if (U1STAbits.PERR) {
    u8_c = U1RXREG; //clear error
    reportError("UART1 parity error\n");
  }
  if (U1STAbits.FERR) {
    u8_c = U1RXREG; //clear error
    reportError("UART1 framing error\n");
  }
  if (U1STAbits.OERR) {
    U1STAbits.OERR = 0; //clear error
    reportError("UART1 overrun error\n");
  }
}




#ifdef UART1_TX_INTERRUPT
/**

*/
#ifndef UART1_TX_FIFO_SIZE
#define UART1_TX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART1_TX_INTERRUPT_PRIORITY
#define UART1_TX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_txFifo1[UART1_TX_FIFO_SIZE];
volatile uint16_t u16_txFifo1Head = 0;
volatile uint16_t u16_txFifo1Tail = 0;

/**
* Output \em u8_c to UART1 TX.
* \param u8_c Character to write
*/
void outChar1(uint8_t u8_c) {
  uint16_t u16_tmp;

  u16_tmp = u16_txFifo1Head;
  u16_tmp++;
  if (u16_tmp == UART1_TX_FIFO_SIZE) u16_tmp = 0; //wrap if needed
  while (u16_tmp == u16_txFifo1Tail)
    doHeartbeat();

  au8_txFifo1[u16_tmp] = u8_c; //write to buffer
  u16_txFifo1Head = u16_tmp;  //update head
  _U1TXIE = 1;               //enable interrupt
}

void _ISR _U1TXInterrupt (void) {
  if (u16_txFifo1Head == u16_txFifo1Tail) {
    //empty TX buffer, disable the interrupt, do not clear the flag
    _U1TXIE = 0;
  } else {
    //at least one free spot in the TX buffer!
    u16_txFifo1Tail++;   //increment tail pointer
    if (u16_txFifo1Tail == UART1_TX_FIFO_SIZE)
      u16_txFifo1Tail = 0; //wrap if needed
    _U1TXIF = 0; //clear the interrupt flag
    //transfer character from software buffer to transmit buffer
    U1TXREG =  au8_txFifo1[u16_txFifo1Tail];
  }
}


#else
/**
* Output \em u8_c to UART1 TX.
* \param u8_c Character to write
*/
void outChar1(uint8_t u8_c) {
  //wait for transmit buffer to be empty
  while (IS_TRANSMIT_BUFFER_FULL_UART1())
    doHeartbeat();
  U1TXREG = u8_c;
}
#endif

#ifdef UART1_RX_INTERRUPT
//Interrupt driven RX
#ifndef UART1_RX_FIFO_SIZE
#define UART1_RX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART1_RX_INTERRUPT_PRIORITY
#define UART1_RX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_rxFifo1[UART1_RX_FIFO_SIZE];
volatile uint16_t u16_rxFifo1Head = 0;
volatile uint16_t u16_rxFifo1Tail = 0;

/**
* Return true if character is ready to be read
*/
uint8_t isCharReady1(void) {
  return(u16_rxFifo1Head != u16_rxFifo1Tail);
}

/**
* Wait for a byte to be available from UART1 RX.
* \return Character read from UART1 RX.
*/
uint8_t inChar1(void) {
  while (u16_rxFifo1Head == u16_rxFifo1Tail)
    doHeartbeat();
  u16_rxFifo1Tail++;
  if (u16_rxFifo1Tail == UART1_RX_FIFO_SIZE) u16_rxFifo1Tail=0; //wrap
  return au8_rxFifo1[u16_rxFifo1Tail]; //return the character
}

void _ISR _U1RXInterrupt (void) {
  int8_t u8_c;

  _U1RXIF = 0;        //clear the UART RX interrupt bit
  checkRxErrorUART1();
  u8_c = U1RXREG;     //read character
  u16_rxFifo1Head++;   //increment head pointer
  if (u16_rxFifo1Head == UART1_RX_FIFO_SIZE)
    u16_rxFifo1Head = 0; //wrap if needed
  if (u16_rxFifo1Head == u16_rxFifo1Tail) {
    //FIFO overrun!, report error
    reportError("UART1 RX Interrupt FIFO overrun!");
  }
  au8_rxFifo1[u16_rxFifo1Head] = u8_c; //place in buffer
}

#else
/**
* Return true if character is ready to be read
*/
uint8_t isCharReady1(void) {
  return(IS_CHAR_READY_UART1());
}

/**
* Wait for a byte to be available from UART1 RX.
* \return Character read from UART1 RX.
*/
uint8_t inChar1(void) {
  //do heartbeat while waiting for character.
  // Use a do-while to insure error checks
  // are always run.
  while (!IS_CHAR_READY_UART1())
    doHeartbeat();
  checkRxErrorUART1();
  return U1RXREG; //read the receive register
}
#endif



/** Chose a default BRGH for UART1, used by
 *  \ref configUART1 to set up UART1.
 */
#ifndef DEFAULT_BRGH1
#define DEFAULT_BRGH1  DEFAULT_BRGH
#endif

#if (DEFAULT_BRGH1 != 0) && (DEFAULT_BRGH1 != 1)
#error Invalid value specified for DEFAULT_BRGH1
#endif


/** Configure the UART. Settings chosen:
 *  - TX is on RP11
 *  - RX is on RP10
 *  - Format is 8 data bits, no parity, 1 stop bit
 *  - CTS, RTS, and BCLK not used
 *
 *  \param u32_baudRate The baud rate to use.
 */
void configUART1(uint32_t u32_baudRate) {
#ifdef _NOFLOAT
  uint32_t u32_brg;
#else
  float f_brg;
#endif

  /*************************  UART config ********************/
  // See comments in the #warning statements below for
  // instructions on how to modify this configuration.
  // More information on each of these board is given
  // in the HARDWARE_PLATFORM section of pic24_libconfig.h.
#if (HARDWARE_PLATFORM == EXPLORER16_100P)
  // There's nothing to do, since pins on these boards are
  // mapped to fixed ports.
#elif (1 == 1)
  // The following pin mappings will apply only to UART 1.
  // Change them as necessary for your device.
 #if (HARDWARE_PLATFORM == DANGEROUS_WEB)
  CONFIG_RP14_AS_DIG_PIN();             // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(14);                // U1RX <- RP14
  CONFIG_RP15_AS_DIG_PIN();             // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(15);                // U1TX -> RP15
  CONFIG_RP10_AS_DIG_PIN();             // RX RP pin must be digital
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == STARTER_BOARD_28P)
  CONFIG_RP9_AS_DIG_PIN();              // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(9);                 // U1RX <- RP9
  CONFIG_RP8_AS_DIG_PIN();              // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(8);                 // U1TX -> RP8
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == DEFAULT_DESIGN)
  CONFIG_U1RX_TO_RP(10);               //U1RX <- RP10
  CONFIG_RP11_AS_DIG_PIN();                //TX RP pin must be digital
  CONFIG_U1TX_TO_RP(11);               //U1TX -> RP11
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #else
  #error Unknown hardware platform.
 #endif
#else
  #warning UART1 pin mappings not defined. See comments below for more info.
  // If your device has more than
  // one UART, ****** CHANGE THE MAPPING ****** since
  // multiple UARTs can not share the same pins.
  // In particular:
  // 1. Change the statement #if (1 == 1) to #if 1
  // 2. Change the pin numbers in the next four lines
  //    to something valid for your device.
  //    If your device does not have remappable I/O,
  //    (typical for >44 pin packages), skip this step --
  //    the UART I/O pins are already assigned to something
  //    valid.
#endif

  // First, disable the UART, clearing all errors, terminating
  // any in-progress transmissions/receptions, etc.
  // In particular, this clears UTXEN.
  U1MODE = (0u << 15); // UARTEN = 0 to disable.

  /* Configure UART baud rate, based on \ref FCY.
   *  NOTE: Be careful about using BRGH=1 - this uses only four clock
   *  periods to sample each bit and can be very intolerant of
   *  baud rate % error - you may see framing errors. BRGH is selected
   *  via the DEFAULT_BRGH1 define above.
   */
#ifdef _NOFLOAT
  u32_brg = FCY/u32_baudRate;
#if (DEFAULT_BRGH1 == 0)
  if ((u32_brg & 0x0FL) >= 8) u32_brg = u32_brg/16;
  else u32_brg = u32_brg/16 - 1;
#else
  if ((brg & 0x03L) >= 2) u32_brg = u32_brg/4;
  else u32_brg = u32_brg/4 - 1;
#endif
  ASSERT(u32_brg < 65536);
  U1BRG = u32_brg;
#else
#if (DEFAULT_BRGH1 == 0)
  f_brg = (((float) FCY)/((float) u32_baudRate)/16.0) - 1.0;
#else
  f_brg = (((float) FCY)/((float) u32_baudRate)/4.0) - 1.0;
#endif
  ASSERT(f_brg < 65535.5);
  U1BRG = roundFloatToUint16(f_brg);
#endif

  // Set up the UART mode register
  U1MODE =
    (1u   << 15) |      // UARTEN = 1 (enable the UART)
    (0u   << 13) |      // USIDL = 0  (continue operation in idle mode)
    (0u   << 12) |      // IREN = 0   (IrDA encoder and decoder disabled)
    (0u   << 11) |      // RTSMD = 0  (UxRTS# in flow control mode. Given
                        //  the UEN setting below, this doesn't matter.)
    (0b00 <<  8) |      // UEN = 00   (UxTX and UxRx pins are enabled and used;
                        //  used; UxCTS, UxRTS, and BCLKx pins are
                        //  controlled by port latches)
    (0u   <<  7) |      // WAKE = 0   (Wake-up on start bit detect during
                        //  sleep mode disabled)
    (0u   <<  6) |      // LPBACK = 0 (UARTx loopback mode is disabled)
    (0u   <<  5) |      // ABAUD = 0  (Auto-baud disabled)
    (0u   <<  4) |      // URXINV = 0 (Receve polarity inversion bit:
                        //  UxRX idle state is 1)
    (DEFAULT_BRGH1 << 3) | // BRGH (High/low baud rate select bit:
                        //  1 = high speed, 0 = low speed)
    (0b00 <<  1) |      // PDSEL = 00 (8-bit data, no parity)
    (0u   <<  0);       // STSEL = 0  (1 stop bit)

  // Set up the UART status and control register
  U1STA =
    (0u   << 15) |      // UTXISEL1 = 0  (See bit 13 below for explanation)
    (0u   << 14) |      // UTXINV = 0    (UxTX idle state is 1 (though docs
                        //  say 0))
    (0u   << 13) |      // UTXISEL0 = 0  (With bit 15 above, UTXISEL = 00:
                        //  Interrupt generated when any character
                        //  is transferred to the Transmit Shift Register).
    (0u   << 11) |      // UTXBRK = 0    (Sync break transmission disabled)
    (1u   << 10) |      // UTXEN = 0     (UARTx transmitter enabled. NOTE: per
                        //  the data sheet, this must be set *AFTER* UARTEN
                        //  is set to 1 (see UxMODE assignment above).
    (0b00 <<  6) |      // URXISEL = 00  (Interrupt flag bit is set when a
                        //  character is received)
    (0u   <<  5) |      // ADDEN = 0     (Address detect mode disabled)
    (0u   <<  1);       // OERR = 0      (Clear any overrun errors)

  // Delay 1 ms (at least one bit time) before beginning transmission,
  // since the UxTX pin is initially configured as an input on device
  // reset. It needs to be high for at least one bit time before
  // a character is sent in order for the start bit to be recognized.
  // See 24H FRM Sec. 17.5 for more details.
  ASSERT(u32_baudRate >= 1000L);
  // Note: If the baud rate is less than 1000, this delay
  // isn't long encough. The alternative is to use floating-point
  // math to compute the correct delay, which seems worse than the
  // problem.
  DELAY_MS(1);

  // Clear any errors
  U1STAbits.OERR = 0;
  while (U1STAbits.PERR || U1STAbits.FERR) {
    U1RXREG; //clear errors by reading RXREG
  }

  // Set up interrupts if requested
#ifdef UART1_RX_INTERRUPT
  _U1RXIF = 0;            //clear the flag
  _U1RXIP = UART1_RX_INTERRUPT_PRIORITY; //choose a priority
  _U1RXIE = 1;            //enable the interrupt
#endif
#ifdef UART1_TX_INTERRUPT
  //do not clear the U1TXIF flag!
  _U1TXIP = UART1_TX_INTERRUPT_PRIORITY; //choose a priority
  //do not enable the interrupt until we try to write to the UART
#endif

}

#endif // #if (NUM_UARTS >= 1)




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




#include "pic24_clockfreq.h"
#include "pic24_uart.h"
#include "pic24_ports.h"
#include "pic24_unittest.h"
#include "pic24_delay.h"

// Only include if this UART exists.
#if (NUM_UART_MODS >= 2)


// Documentation for this file. If the \file tag is not present,
// this file will not be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
*  UART support functions.
*  \par Interrupt-driven TX/RX
*  By default, the UART functions use polling for both RX and TX.
*  Define the macro UARTx_TX_INTERRUPT (i.e., UART1_TX_INTERRUPT) in your project file if you want interrupt-driven TX for the UARTx (i.e., UART1) module.
*  For interrupt-driven TX, macro UARTx_TX_FIFO_SIZE sets the TX software FIFO size (default 32), and UARTx_TX_INTERRUPT_PRIORITY  sets the priority (default 1).
* \par
*  Define the macro UARTx_RX_INTERRUPT (i.e., UART1_RX_INTERRUPT) in your project file if you want interrupt-driven RX for the UARTx (i.e., UART1) module.
*  For interrupt-driven RX, macro UARTx_RX_FIFO_SIZE sets the RX software FIFO size (default 32), and UARTx_RX_INTERRUPT_PRIORITY  sets the priority (default 1).
*/


/*********************************
 * Function private to this file *
 *********************************/






/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/
/**
* Check UART2 RX for error, call \em reportError() if error found.
*
*/
void checkRxErrorUART2(void) {
  uint8_t u8_c;
//check for errors, reset if detected.
  if (U2STAbits.PERR) {
    u8_c = U2RXREG; //clear error
    reportError("UART2 parity error\n");
  }
  if (U2STAbits.FERR) {
    u8_c = U2RXREG; //clear error
    reportError("UART2 framing error\n");
  }
  if (U2STAbits.OERR) {
    U2STAbits.OERR = 0; //clear error
    reportError("UART2 overrun error\n");
  }
}




#ifdef UART2_TX_INTERRUPT
/**

*/
#ifndef UART2_TX_FIFO_SIZE
#define UART2_TX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART2_TX_INTERRUPT_PRIORITY
#define UART2_TX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_txFifo2[UART2_TX_FIFO_SIZE];
volatile uint16_t u16_txFifo2Head = 0;
volatile uint16_t u16_txFifo2Tail = 0;

/**
* Output \em u8_c to UART2 TX.
* \param u8_c Character to write
*/
void outChar2(uint8_t u8_c) {
  uint16_t u16_tmp;

  u16_tmp = u16_txFifo2Head;
  u16_tmp++;
  if (u16_tmp == UART2_TX_FIFO_SIZE) u16_tmp = 0; //wrap if needed
  while (u16_tmp == u16_txFifo2Tail)
    doHeartbeat();

  au8_txFifo2[u16_tmp] = u8_c; //write to buffer
  u16_txFifo2Head = u16_tmp;  //update head
  _U2TXIE = 1;               //enable interrupt
}

void _ISR _U2TXInterrupt (void) {
  if (u16_txFifo2Head == u16_txFifo2Tail) {
    //empty TX buffer, disable the interrupt, do not clear the flag
    _U2TXIE = 0;
  } else {
    //at least one free spot in the TX buffer!
    u16_txFifo2Tail++;   //increment tail pointer
    if (u16_txFifo2Tail == UART2_TX_FIFO_SIZE)
      u16_txFifo2Tail = 0; //wrap if needed
    _U2TXIF = 0; //clear the interrupt flag
    //transfer character from software buffer to transmit buffer
    U2TXREG =  au8_txFifo2[u16_txFifo2Tail];
  }
}


#else
/**
* Output \em u8_c to UART2 TX.
* \param u8_c Character to write
*/
void outChar2(uint8_t u8_c) {
  //wait for transmit buffer to be empty
  while (IS_TRANSMIT_BUFFER_FULL_UART2())
    doHeartbeat();
  U2TXREG = u8_c;
}
#endif

#ifdef UART2_RX_INTERRUPT
//Interrupt driven RX
#ifndef UART2_RX_FIFO_SIZE
#define UART2_RX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART2_RX_INTERRUPT_PRIORITY
#define UART2_RX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_rxFifo2[UART2_RX_FIFO_SIZE];
volatile uint16_t u16_rxFifo2Head = 0;
volatile uint16_t u16_rxFifo2Tail = 0;

/**
* Return true if character is ready to be read
*/
uint8_t isCharReady2(void) {
  return(u16_rxFifo2Head != u16_rxFifo2Tail);
}

/**
* Wait for a byte to be available from UART2 RX.
* \return Character read from UART2 RX.
*/
uint8_t inChar2(void) {
  while (u16_rxFifo2Head == u16_rxFifo2Tail)
    doHeartbeat();
  u16_rxFifo2Tail++;
  if (u16_rxFifo2Tail == UART2_RX_FIFO_SIZE) u16_rxFifo2Tail=0; //wrap
  return au8_rxFifo2[u16_rxFifo2Tail]; //return the character
}

void _ISR _U2RXInterrupt (void) {
  int8_t u8_c;

  _U2RXIF = 0;        //clear the UART RX interrupt bit
  checkRxErrorUART2();
  u8_c = U2RXREG;     //read character
  u16_rxFifo2Head++;   //increment head pointer
  if (u16_rxFifo2Head == UART2_RX_FIFO_SIZE)
    u16_rxFifo2Head = 0; //wrap if needed
  if (u16_rxFifo2Head == u16_rxFifo2Tail) {
    //FIFO overrun!, report error
    reportError("UART2 RX Interrupt FIFO overrun!");
  }
  au8_rxFifo2[u16_rxFifo2Head] = u8_c; //place in buffer
}

#else
/**
* Return true if character is ready to be read
*/
uint8_t isCharReady2(void) {
  return(IS_CHAR_READY_UART2());
}

/**
* Wait for a byte to be available from UART2 RX.
* \return Character read from UART2 RX.
*/
uint8_t inChar2(void) {
  //do heartbeat while waiting for character.
  // Use a do-while to insure error checks
  // are always run.
  while (!IS_CHAR_READY_UART2())
    doHeartbeat();
  checkRxErrorUART2();
  return U2RXREG; //read the receive register
}
#endif



/** Chose a default BRGH for UART2, used by
 *  \ref configUART2 to set up UART2.
 */
#ifndef DEFAULT_BRGH2
#define DEFAULT_BRGH2  DEFAULT_BRGH
#endif

#if (DEFAULT_BRGH2 != 0) && (DEFAULT_BRGH2 != 1)
#error Invalid value specified for DEFAULT_BRGH2
#endif


/** Configure the UART. Settings chosen:
 *  - TX is on RP11
 *  - RX is on RP10
 *  - Format is 8 data bits, no parity, 1 stop bit
 *  - CTS, RTS, and BCLK not used
 *
 *  \param u32_baudRate The baud rate to use.
 */
void configUART2(uint32_t u32_baudRate) {
#ifdef _NOFLOAT
  uint32_t u32_brg;
#else
  float f_brg;
#endif

  /*************************  UART config ********************/
  // See comments in the #warning statements below for
  // instructions on how to modify this configuration.
  // More information on each of these board is given
  // in the HARDWARE_PLATFORM section of pic24_libconfig.h.
#if (HARDWARE_PLATFORM == EXPLORER16_100P)
  // There's nothing to do, since pins on these boards are
  // mapped to fixed ports.
#elif (2 == 1)
  // The following pin mappings will apply only to UART 1.
  // Change them as necessary for your device.
 #if (HARDWARE_PLATFORM == DANGEROUS_WEB)
  CONFIG_RP14_AS_DIG_PIN();             // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(14);                // U1RX <- RP14
  CONFIG_RP15_AS_DIG_PIN();             // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(15);                // U1TX -> RP15
  CONFIG_RP10_AS_DIG_PIN();             // RX RP pin must be digital
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == STARTER_BOARD_28P)
  CONFIG_RP9_AS_DIG_PIN();              // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(9);                 // U1RX <- RP9
  CONFIG_RP8_AS_DIG_PIN();              // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(8);                 // U1TX -> RP8
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == DEFAULT_DESIGN)
  CONFIG_U2RX_TO_RP(10);               //U2RX <- RP10
  CONFIG_RP11_AS_DIG_PIN();                //TX RP pin must be digital
  CONFIG_U2TX_TO_RP(11);               //U2TX -> RP11
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #else
  #error Unknown hardware platform.
 #endif
#else
  #warning UART2 pin mappings not defined. See comments below for more info.
  // If your device has more than
  // one UART, ****** CHANGE THE MAPPING ****** since
  // multiple UARTs can not share the same pins.
  // In particular:
  // 1. Change the statement #if (2 == 1) to #if 1
  // 2. Change the pin numbers in the next four lines
  //    to something valid for your device.
  //    If your device does not have remappable I/O,
  //    (typical for >44 pin packages), skip this step --
  //    the UART I/O pins are already assigned to something
  //    valid.
#endif

  // First, disable the UART, clearing all errors, terminating
  // any in-progress transmissions/receptions, etc.
  // In particular, this clears UTXEN.
  U2MODE = (0u << 15); // UARTEN = 0 to disable.

  /* Configure UART baud rate, based on \ref FCY.
   *  NOTE: Be careful about using BRGH=1 - this uses only four clock
   *  periods to sample each bit and can be very intolerant of
   *  baud rate % error - you may see framing errors. BRGH is selected
   *  via the DEFAULT_BRGH1 define above.
   */
#ifdef _NOFLOAT
  u32_brg = FCY/u32_baudRate;
#if (DEFAULT_BRGH2 == 0)
  if ((u32_brg & 0x0FL) >= 8) u32_brg = u32_brg/16;
  else u32_brg = u32_brg/16 - 1;
#else
  if ((brg & 0x03L) >= 2) u32_brg = u32_brg/4;
  else u32_brg = u32_brg/4 - 1;
#endif
  ASSERT(u32_brg < 65536);
  U2BRG = u32_brg;
#else
#if (DEFAULT_BRGH2 == 0)
  f_brg = (((float) FCY)/((float) u32_baudRate)/16.0) - 1.0;
#else
  f_brg = (((float) FCY)/((float) u32_baudRate)/4.0) - 1.0;
#endif
  ASSERT(f_brg < 65535.5);
  U2BRG = roundFloatToUint16(f_brg);
#endif

  // Set up the UART mode register
  U2MODE =
    (1u   << 15) |      // UARTEN = 1 (enable the UART)
    (0u   << 13) |      // USIDL = 0  (continue operation in idle mode)
    (0u   << 12) |      // IREN = 0   (IrDA encoder and decoder disabled)
    (0u   << 11) |      // RTSMD = 0  (UxRTS# in flow control mode. Given
                        //  the UEN setting below, this doesn't matter.)
    (0b00 <<  8) |      // UEN = 00   (UxTX and UxRx pins are enabled and used;
                        //  used; UxCTS, UxRTS, and BCLKx pins are
                        //  controlled by port latches)
    (0u   <<  7) |      // WAKE = 0   (Wake-up on start bit detect during
                        //  sleep mode disabled)
    (0u   <<  6) |      // LPBACK = 0 (UARTx loopback mode is disabled)
    (0u   <<  5) |      // ABAUD = 0  (Auto-baud disabled)
    (0u   <<  4) |      // URXINV = 0 (Receve polarity inversion bit:
                        //  UxRX idle state is 1)
    (DEFAULT_BRGH2 << 3) | // BRGH (High/low baud rate select bit:
                        //  1 = high speed, 0 = low speed)
    (0b00 <<  1) |      // PDSEL = 00 (8-bit data, no parity)
    (0u   <<  0);       // STSEL = 0  (1 stop bit)

  // Set up the UART status and control register
  U2STA =
    (0u   << 15) |      // UTXISEL1 = 0  (See bit 13 below for explanation)
    (0u   << 14) |      // UTXINV = 0    (UxTX idle state is 1 (though docs
                        //  say 0))
    (0u   << 13) |      // UTXISEL0 = 0  (With bit 15 above, UTXISEL = 00:
                        //  Interrupt generated when any character
                        //  is transferred to the Transmit Shift Register).
    (0u   << 11) |      // UTXBRK = 0    (Sync break transmission disabled)
    (1u   << 10) |      // UTXEN = 0     (UARTx transmitter enabled. NOTE: per
                        //  the data sheet, this must be set *AFTER* UARTEN
                        //  is set to 1 (see UxMODE assignment above).
    (0b00 <<  6) |      // URXISEL = 00  (Interrupt flag bit is set when a
                        //  character is received)
    (0u   <<  5) |      // ADDEN = 0     (Address detect mode disabled)
    (0u   <<  1);       // OERR = 0      (Clear any overrun errors)

  // Delay 1 ms (at least one bit time) before beginning transmission,
  // since the UxTX pin is initially configured as an input on device
  // reset. It needs to be high for at least one bit time before
  // a character is sent in order for the start bit to be recognized.
  // See 24H FRM Sec. 17.5 for more details.
  ASSERT(u32_baudRate >= 1000L);
  // Note: If the baud rate is less than 1000, this delay
  // isn't long encough. The alternative is to use floating-point
  // math to compute the correct delay, which seems worse than the
  // problem.
  DELAY_MS(1);

  // Clear any errors
  U2STAbits.OERR = 0;
  while (U2STAbits.PERR || U2STAbits.FERR) {
    U2RXREG; //clear errors by reading RXREG
  }

  // Set up interrupts if requested
#ifdef UART2_RX_INTERRUPT
  _U2RXIF = 0;            //clear the flag
  _U2RXIP = UART2_RX_INTERRUPT_PRIORITY; //choose a priority
  _U2RXIE = 1;            //enable the interrupt
#endif
#ifdef UART2_TX_INTERRUPT
  //do not clear the U2TXIF flag!
  _U2TXIP = UART2_TX_INTERRUPT_PRIORITY; //choose a priority
  //do not enable the interrupt until we try to write to the UART
#endif

}

#endif // #if (NUM_UARTS >= 2)




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




#include "pic24_clockfreq.h"
#include "pic24_uart.h"
#include "pic24_ports.h"
#include "pic24_unittest.h"
#include "pic24_delay.h"

// Only include if this UART exists.
#if (NUM_UART_MODS >= 3)


// Documentation for this file. If the \file tag is not present,
// this file will not be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
*  UART support functions.
*  \par Interrupt-driven TX/RX
*  By default, the UART functions use polling for both RX and TX.
*  Define the macro UARTx_TX_INTERRUPT (i.e., UART1_TX_INTERRUPT) in your project file if you want interrupt-driven TX for the UARTx (i.e., UART1) module.
*  For interrupt-driven TX, macro UARTx_TX_FIFO_SIZE sets the TX software FIFO size (default 32), and UARTx_TX_INTERRUPT_PRIORITY  sets the priority (default 1).
* \par
*  Define the macro UARTx_RX_INTERRUPT (i.e., UART1_RX_INTERRUPT) in your project file if you want interrupt-driven RX for the UARTx (i.e., UART1) module.
*  For interrupt-driven RX, macro UARTx_RX_FIFO_SIZE sets the RX software FIFO size (default 32), and UARTx_RX_INTERRUPT_PRIORITY  sets the priority (default 1).
*/


/*********************************
 * Function private to this file *
 *********************************/






/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/
/**
* Check UART3 RX for error, call \em reportError() if error found.
*
*/
void checkRxErrorUART3(void) {
  uint8_t u8_c;
//check for errors, reset if detected.
  if (U3STAbits.PERR) {
    u8_c = U3RXREG; //clear error
    reportError("UART3 parity error\n");
  }
  if (U3STAbits.FERR) {
    u8_c = U3RXREG; //clear error
    reportError("UART3 framing error\n");
  }
  if (U3STAbits.OERR) {
    U3STAbits.OERR = 0; //clear error
    reportError("UART3 overrun error\n");
  }
}




#ifdef UART3_TX_INTERRUPT
/**

*/
#ifndef UART3_TX_FIFO_SIZE
#define UART3_TX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART3_TX_INTERRUPT_PRIORITY
#define UART3_TX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_txFifo3[UART3_TX_FIFO_SIZE];
volatile uint16_t u16_txFifo3Head = 0;
volatile uint16_t u16_txFifo3Tail = 0;

/**
* Output \em u8_c to UART3 TX.
* \param u8_c Character to write
*/
void outChar3(uint8_t u8_c) {
  uint16_t u16_tmp;

  u16_tmp = u16_txFifo3Head;
  u16_tmp++;
  if (u16_tmp == UART3_TX_FIFO_SIZE) u16_tmp = 0; //wrap if needed
  while (u16_tmp == u16_txFifo3Tail)
    doHeartbeat();

  au8_txFifo3[u16_tmp] = u8_c; //write to buffer
  u16_txFifo3Head = u16_tmp;  //update head
  _U3TXIE = 1;               //enable interrupt
}

void _ISR _U3TXInterrupt (void) {
  if (u16_txFifo3Head == u16_txFifo3Tail) {
    //empty TX buffer, disable the interrupt, do not clear the flag
    _U3TXIE = 0;
  } else {
    //at least one free spot in the TX buffer!
    u16_txFifo3Tail++;   //increment tail pointer
    if (u16_txFifo3Tail == UART3_TX_FIFO_SIZE)
      u16_txFifo3Tail = 0; //wrap if needed
    _U3TXIF = 0; //clear the interrupt flag
    //transfer character from software buffer to transmit buffer
    U3TXREG =  au8_txFifo3[u16_txFifo3Tail];
  }
}


#else
/**
* Output \em u8_c to UART3 TX.
* \param u8_c Character to write
*/
void outChar3(uint8_t u8_c) {
  //wait for transmit buffer to be empty
  while (IS_TRANSMIT_BUFFER_FULL_UART3())
    doHeartbeat();
  U3TXREG = u8_c;
}
#endif

#ifdef UART3_RX_INTERRUPT
//Interrupt driven RX
#ifndef UART3_RX_FIFO_SIZE
#define UART3_RX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART3_RX_INTERRUPT_PRIORITY
#define UART3_RX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_rxFifo3[UART3_RX_FIFO_SIZE];
volatile uint16_t u16_rxFifo3Head = 0;
volatile uint16_t u16_rxFifo3Tail = 0;

/**
* Return true if character is ready to be read
*/
uint8_t isCharReady3(void) {
  return(u16_rxFifo3Head != u16_rxFifo3Tail);
}

/**
* Wait for a byte to be available from UART3 RX.
* \return Character read from UART3 RX.
*/
uint8_t inChar3(void) {
  while (u16_rxFifo3Head == u16_rxFifo3Tail)
    doHeartbeat();
  u16_rxFifo3Tail++;
  if (u16_rxFifo3Tail == UART3_RX_FIFO_SIZE) u16_rxFifo3Tail=0; //wrap
  return au8_rxFifo3[u16_rxFifo3Tail]; //return the character
}

void _ISR _U3RXInterrupt (void) {
  int8_t u8_c;

  _U3RXIF = 0;        //clear the UART RX interrupt bit
  checkRxErrorUART3();
  u8_c = U3RXREG;     //read character
  u16_rxFifo3Head++;   //increment head pointer
  if (u16_rxFifo3Head == UART3_RX_FIFO_SIZE)
    u16_rxFifo3Head = 0; //wrap if needed
  if (u16_rxFifo3Head == u16_rxFifo3Tail) {
    //FIFO overrun!, report error
    reportError("UART3 RX Interrupt FIFO overrun!");
  }
  au8_rxFifo3[u16_rxFifo3Head] = u8_c; //place in buffer
}

#else
/**
* Return true if character is ready to be read
*/
uint8_t isCharReady3(void) {
  return(IS_CHAR_READY_UART3());
}

/**
* Wait for a byte to be available from UART3 RX.
* \return Character read from UART3 RX.
*/
uint8_t inChar3(void) {
  //do heartbeat while waiting for character.
  // Use a do-while to insure error checks
  // are always run.
  while (!IS_CHAR_READY_UART3())
    doHeartbeat();
  checkRxErrorUART3();
  return U3RXREG; //read the receive register
}
#endif



/** Chose a default BRGH for UART3, used by
 *  \ref configUART3 to set up UART3.
 */
#ifndef DEFAULT_BRGH3
#define DEFAULT_BRGH3  DEFAULT_BRGH
#endif

#if (DEFAULT_BRGH3 != 0) && (DEFAULT_BRGH3 != 1)
#error Invalid value specified for DEFAULT_BRGH3
#endif


/** Configure the UART. Settings chosen:
 *  - TX is on RP11
 *  - RX is on RP10
 *  - Format is 8 data bits, no parity, 1 stop bit
 *  - CTS, RTS, and BCLK not used
 *
 *  \param u32_baudRate The baud rate to use.
 */
void configUART3(uint32_t u32_baudRate) {
#ifdef _NOFLOAT
  uint32_t u32_brg;
#else
  float f_brg;
#endif

  /*************************  UART config ********************/
  // See comments in the #warning statements below for
  // instructions on how to modify this configuration.
  // More information on each of these board is given
  // in the HARDWARE_PLATFORM section of pic24_libconfig.h.
#if (HARDWARE_PLATFORM == EXPLORER16_100P)
  // There's nothing to do, since pins on these boards are
  // mapped to fixed ports.
#elif (3 == 1)
  // The following pin mappings will apply only to UART 1.
  // Change them as necessary for your device.
 #if (HARDWARE_PLATFORM == DANGEROUS_WEB)
  CONFIG_RP14_AS_DIG_PIN();             // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(14);                // U1RX <- RP14
  CONFIG_RP15_AS_DIG_PIN();             // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(15);                // U1TX -> RP15
  CONFIG_RP10_AS_DIG_PIN();             // RX RP pin must be digital
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == STARTER_BOARD_28P)
  CONFIG_RP9_AS_DIG_PIN();              // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(9);                 // U1RX <- RP9
  CONFIG_RP8_AS_DIG_PIN();              // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(8);                 // U1TX -> RP8
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == DEFAULT_DESIGN)
  CONFIG_U3RX_TO_RP(10);               //U3RX <- RP10
  CONFIG_RP11_AS_DIG_PIN();                //TX RP pin must be digital
  CONFIG_U3TX_TO_RP(11);               //U3TX -> RP11
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #else
  #error Unknown hardware platform.
 #endif
#else
  #warning UART3 pin mappings not defined. See comments below for more info.
  // If your device has more than
  // one UART, ****** CHANGE THE MAPPING ****** since
  // multiple UARTs can not share the same pins.
  // In particular:
  // 1. Change the statement #if (3 == 1) to #if 1
  // 2. Change the pin numbers in the next four lines
  //    to something valid for your device.
  //    If your device does not have remappable I/O,
  //    (typical for >44 pin packages), skip this step --
  //    the UART I/O pins are already assigned to something
  //    valid.
#endif

  // First, disable the UART, clearing all errors, terminating
  // any in-progress transmissions/receptions, etc.
  // In particular, this clears UTXEN.
  U3MODE = (0u << 15); // UARTEN = 0 to disable.

  /* Configure UART baud rate, based on \ref FCY.
   *  NOTE: Be careful about using BRGH=1 - this uses only four clock
   *  periods to sample each bit and can be very intolerant of
   *  baud rate % error - you may see framing errors. BRGH is selected
   *  via the DEFAULT_BRGH1 define above.
   */
#ifdef _NOFLOAT
  u32_brg = FCY/u32_baudRate;
#if (DEFAULT_BRGH3 == 0)
  if ((u32_brg & 0x0FL) >= 8) u32_brg = u32_brg/16;
  else u32_brg = u32_brg/16 - 1;
#else
  if ((brg & 0x03L) >= 2) u32_brg = u32_brg/4;
  else u32_brg = u32_brg/4 - 1;
#endif
  ASSERT(u32_brg < 65536);
  U3BRG = u32_brg;
#else
#if (DEFAULT_BRGH3 == 0)
  f_brg = (((float) FCY)/((float) u32_baudRate)/16.0) - 1.0;
#else
  f_brg = (((float) FCY)/((float) u32_baudRate)/4.0) - 1.0;
#endif
  ASSERT(f_brg < 65535.5);
  U3BRG = roundFloatToUint16(f_brg);
#endif

  // Set up the UART mode register
  U3MODE =
    (1u   << 15) |      // UARTEN = 1 (enable the UART)
    (0u   << 13) |      // USIDL = 0  (continue operation in idle mode)
    (0u   << 12) |      // IREN = 0   (IrDA encoder and decoder disabled)
    (0u   << 11) |      // RTSMD = 0  (UxRTS# in flow control mode. Given
                        //  the UEN setting below, this doesn't matter.)
    (0b00 <<  8) |      // UEN = 00   (UxTX and UxRx pins are enabled and used;
                        //  used; UxCTS, UxRTS, and BCLKx pins are
                        //  controlled by port latches)
    (0u   <<  7) |      // WAKE = 0   (Wake-up on start bit detect during
                        //  sleep mode disabled)
    (0u   <<  6) |      // LPBACK = 0 (UARTx loopback mode is disabled)
    (0u   <<  5) |      // ABAUD = 0  (Auto-baud disabled)
    (0u   <<  4) |      // URXINV = 0 (Receve polarity inversion bit:
                        //  UxRX idle state is 1)
    (DEFAULT_BRGH3 << 3) | // BRGH (High/low baud rate select bit:
                        //  1 = high speed, 0 = low speed)
    (0b00 <<  1) |      // PDSEL = 00 (8-bit data, no parity)
    (0u   <<  0);       // STSEL = 0  (1 stop bit)

  // Set up the UART status and control register
  U3STA =
    (0u   << 15) |      // UTXISEL1 = 0  (See bit 13 below for explanation)
    (0u   << 14) |      // UTXINV = 0    (UxTX idle state is 1 (though docs
                        //  say 0))
    (0u   << 13) |      // UTXISEL0 = 0  (With bit 15 above, UTXISEL = 00:
                        //  Interrupt generated when any character
                        //  is transferred to the Transmit Shift Register).
    (0u   << 11) |      // UTXBRK = 0    (Sync break transmission disabled)
    (1u   << 10) |      // UTXEN = 0     (UARTx transmitter enabled. NOTE: per
                        //  the data sheet, this must be set *AFTER* UARTEN
                        //  is set to 1 (see UxMODE assignment above).
    (0b00 <<  6) |      // URXISEL = 00  (Interrupt flag bit is set when a
                        //  character is received)
    (0u   <<  5) |      // ADDEN = 0     (Address detect mode disabled)
    (0u   <<  1);       // OERR = 0      (Clear any overrun errors)

  // Delay 1 ms (at least one bit time) before beginning transmission,
  // since the UxTX pin is initially configured as an input on device
  // reset. It needs to be high for at least one bit time before
  // a character is sent in order for the start bit to be recognized.
  // See 24H FRM Sec. 17.5 for more details.
  ASSERT(u32_baudRate >= 1000L);
  // Note: If the baud rate is less than 1000, this delay
  // isn't long encough. The alternative is to use floating-point
  // math to compute the correct delay, which seems worse than the
  // problem.
  DELAY_MS(1);

  // Clear any errors
  U3STAbits.OERR = 0;
  while (U3STAbits.PERR || U3STAbits.FERR) {
    U3RXREG; //clear errors by reading RXREG
  }

  // Set up interrupts if requested
#ifdef UART3_RX_INTERRUPT
  _U3RXIF = 0;            //clear the flag
  _U3RXIP = UART3_RX_INTERRUPT_PRIORITY; //choose a priority
  _U3RXIE = 1;            //enable the interrupt
#endif
#ifdef UART3_TX_INTERRUPT
  //do not clear the U3TXIF flag!
  _U3TXIP = UART3_TX_INTERRUPT_PRIORITY; //choose a priority
  //do not enable the interrupt until we try to write to the UART
#endif

}

#endif // #if (NUM_UARTS >= 3)




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




#include "pic24_clockfreq.h"
#include "pic24_uart.h"
#include "pic24_ports.h"
#include "pic24_unittest.h"
#include "pic24_delay.h"

// Only include if this UART exists.
#if (NUM_UART_MODS >= 4)


// Documentation for this file. If the \file tag is not present,
// this file will not be documented.
// Note: place this comment below the #if NUM_UART_MODS so Doxygen
// will only see it once.
/** \file
*  UART support functions.
*  \par Interrupt-driven TX/RX
*  By default, the UART functions use polling for both RX and TX.
*  Define the macro UARTx_TX_INTERRUPT (i.e., UART1_TX_INTERRUPT) in your project file if you want interrupt-driven TX for the UARTx (i.e., UART1) module.
*  For interrupt-driven TX, macro UARTx_TX_FIFO_SIZE sets the TX software FIFO size (default 32), and UARTx_TX_INTERRUPT_PRIORITY  sets the priority (default 1).
* \par
*  Define the macro UARTx_RX_INTERRUPT (i.e., UART1_RX_INTERRUPT) in your project file if you want interrupt-driven RX for the UARTx (i.e., UART1) module.
*  For interrupt-driven RX, macro UARTx_RX_FIFO_SIZE sets the RX software FIFO size (default 32), and UARTx_RX_INTERRUPT_PRIORITY  sets the priority (default 1).
*/


/*********************************
 * Function private to this file *
 *********************************/






/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/
/**
* Check UART4 RX for error, call \em reportError() if error found.
*
*/
void checkRxErrorUART4(void) {
  uint8_t u8_c;
//check for errors, reset if detected.
  if (U4STAbits.PERR) {
    u8_c = U4RXREG; //clear error
    reportError("UART4 parity error\n");
  }
  if (U4STAbits.FERR) {
    u8_c = U4RXREG; //clear error
    reportError("UART4 framing error\n");
  }
  if (U4STAbits.OERR) {
    U4STAbits.OERR = 0; //clear error
    reportError("UART4 overrun error\n");
  }
}




#ifdef UART4_TX_INTERRUPT
/**

*/
#ifndef UART4_TX_FIFO_SIZE
#define UART4_TX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART4_TX_INTERRUPT_PRIORITY
#define UART4_TX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_txFifo4[UART4_TX_FIFO_SIZE];
volatile uint16_t u16_txFifo4Head = 0;
volatile uint16_t u16_txFifo4Tail = 0;

/**
* Output \em u8_c to UART4 TX.
* \param u8_c Character to write
*/
void outChar4(uint8_t u8_c) {
  uint16_t u16_tmp;

  u16_tmp = u16_txFifo4Head;
  u16_tmp++;
  if (u16_tmp == UART4_TX_FIFO_SIZE) u16_tmp = 0; //wrap if needed
  while (u16_tmp == u16_txFifo4Tail)
    doHeartbeat();

  au8_txFifo4[u16_tmp] = u8_c; //write to buffer
  u16_txFifo4Head = u16_tmp;  //update head
  _U4TXIE = 1;               //enable interrupt
}

void _ISR _U4TXInterrupt (void) {
  if (u16_txFifo4Head == u16_txFifo4Tail) {
    //empty TX buffer, disable the interrupt, do not clear the flag
    _U4TXIE = 0;
  } else {
    //at least one free spot in the TX buffer!
    u16_txFifo4Tail++;   //increment tail pointer
    if (u16_txFifo4Tail == UART4_TX_FIFO_SIZE)
      u16_txFifo4Tail = 0; //wrap if needed
    _U4TXIF = 0; //clear the interrupt flag
    //transfer character from software buffer to transmit buffer
    U4TXREG =  au8_txFifo4[u16_txFifo4Tail];
  }
}


#else
/**
* Output \em u8_c to UART4 TX.
* \param u8_c Character to write
*/
void outChar4(uint8_t u8_c) {
  //wait for transmit buffer to be empty
  while (IS_TRANSMIT_BUFFER_FULL_UART4())
    doHeartbeat();
  U4TXREG = u8_c;
}
#endif

#ifdef UART4_RX_INTERRUPT
//Interrupt driven RX
#ifndef UART4_RX_FIFO_SIZE
#define UART4_RX_FIFO_SIZE 32  //choose a size
#endif

#ifndef UART4_RX_INTERRUPT_PRIORITY
#define UART4_RX_INTERRUPT_PRIORITY 1
#endif

volatile uint8_t au8_rxFifo4[UART4_RX_FIFO_SIZE];
volatile uint16_t u16_rxFifo4Head = 0;
volatile uint16_t u16_rxFifo4Tail = 0;

/**
* Return true if character is ready to be read
*/
uint8_t isCharReady4(void) {
  return(u16_rxFifo4Head != u16_rxFifo4Tail);
}

/**
* Wait for a byte to be available from UART4 RX.
* \return Character read from UART4 RX.
*/
uint8_t inChar4(void) {
  while (u16_rxFifo4Head == u16_rxFifo4Tail)
    doHeartbeat();
  u16_rxFifo4Tail++;
  if (u16_rxFifo4Tail == UART4_RX_FIFO_SIZE) u16_rxFifo4Tail=0; //wrap
  return au8_rxFifo4[u16_rxFifo4Tail]; //return the character
}

void _ISR _U4RXInterrupt (void) {
  int8_t u8_c;

  _U4RXIF = 0;        //clear the UART RX interrupt bit
  checkRxErrorUART4();
  u8_c = U4RXREG;     //read character
  u16_rxFifo4Head++;   //increment head pointer
  if (u16_rxFifo4Head == UART4_RX_FIFO_SIZE)
    u16_rxFifo4Head = 0; //wrap if needed
  if (u16_rxFifo4Head == u16_rxFifo4Tail) {
    //FIFO overrun!, report error
    reportError("UART4 RX Interrupt FIFO overrun!");
  }
  au8_rxFifo4[u16_rxFifo4Head] = u8_c; //place in buffer
}

#else
/**
* Return true if character is ready to be read
*/
uint8_t isCharReady4(void) {
  return(IS_CHAR_READY_UART4());
}

/**
* Wait for a byte to be available from UART4 RX.
* \return Character read from UART4 RX.
*/
uint8_t inChar4(void) {
  //do heartbeat while waiting for character.
  // Use a do-while to insure error checks
  // are always run.
  while (!IS_CHAR_READY_UART4())
    doHeartbeat();
  checkRxErrorUART4();
  return U4RXREG; //read the receive register
}
#endif



/** Chose a default BRGH for UART4, used by
 *  \ref configUART4 to set up UART4.
 */
#ifndef DEFAULT_BRGH4
#define DEFAULT_BRGH4  DEFAULT_BRGH
#endif

#if (DEFAULT_BRGH4 != 0) && (DEFAULT_BRGH4 != 1)
#error Invalid value specified for DEFAULT_BRGH4
#endif


/** Configure the UART. Settings chosen:
 *  - TX is on RP11
 *  - RX is on RP10
 *  - Format is 8 data bits, no parity, 1 stop bit
 *  - CTS, RTS, and BCLK not used
 *
 *  \param u32_baudRate The baud rate to use.
 */
void configUART4(uint32_t u32_baudRate) {
#ifdef _NOFLOAT
  uint32_t u32_brg;
#else
  float f_brg;
#endif

  /*************************  UART config ********************/
  // See comments in the #warning statements below for
  // instructions on how to modify this configuration.
  // More information on each of these board is given
  // in the HARDWARE_PLATFORM section of pic24_libconfig.h.
#if (HARDWARE_PLATFORM == EXPLORER16_100P)
  // There's nothing to do, since pins on these boards are
  // mapped to fixed ports.
#elif (4 == 1)
  // The following pin mappings will apply only to UART 1.
  // Change them as necessary for your device.
 #if (HARDWARE_PLATFORM == DANGEROUS_WEB)
  CONFIG_RP14_AS_DIG_PIN();             // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(14);                // U1RX <- RP14
  CONFIG_RP15_AS_DIG_PIN();             // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(15);                // U1TX -> RP15
  CONFIG_RP10_AS_DIG_PIN();             // RX RP pin must be digital
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == STARTER_BOARD_28P)
  CONFIG_RP9_AS_DIG_PIN();              // RX RP pin must be digital
  CONFIG_U1RX_TO_RP(9);                 // U1RX <- RP9
  CONFIG_RP8_AS_DIG_PIN();              // TX RP pin must be digital
  CONFIG_U1TX_TO_RP(8);                 // U1TX -> RP8
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #elif (HARDWARE_PLATFORM == DEFAULT_DESIGN)
  CONFIG_U4RX_TO_RP(10);               //U4RX <- RP10
  CONFIG_RP11_AS_DIG_PIN();                //TX RP pin must be digital
  CONFIG_U4TX_TO_RP(11);               //U4TX -> RP11
  // turn off any analog functionality on these pins
  // (may be needed if pin is hardmapped and RPx does
  // not exist)
  DISABLE_U1TX_ANALOG();
  DISABLE_U1RX_ANALOG();
 #else
  #error Unknown hardware platform.
 #endif
#else
  #warning UART4 pin mappings not defined. See comments below for more info.
  // If your device has more than
  // one UART, ****** CHANGE THE MAPPING ****** since
  // multiple UARTs can not share the same pins.
  // In particular:
  // 1. Change the statement #if (4 == 1) to #if 1
  // 2. Change the pin numbers in the next four lines
  //    to something valid for your device.
  //    If your device does not have remappable I/O,
  //    (typical for >44 pin packages), skip this step --
  //    the UART I/O pins are already assigned to something
  //    valid.
#endif

  // First, disable the UART, clearing all errors, terminating
  // any in-progress transmissions/receptions, etc.
  // In particular, this clears UTXEN.
  U4MODE = (0u << 15); // UARTEN = 0 to disable.

  /* Configure UART baud rate, based on \ref FCY.
   *  NOTE: Be careful about using BRGH=1 - this uses only four clock
   *  periods to sample each bit and can be very intolerant of
   *  baud rate % error - you may see framing errors. BRGH is selected
   *  via the DEFAULT_BRGH1 define above.
   */
#ifdef _NOFLOAT
  u32_brg = FCY/u32_baudRate;
#if (DEFAULT_BRGH4 == 0)
  if ((u32_brg & 0x0FL) >= 8) u32_brg = u32_brg/16;
  else u32_brg = u32_brg/16 - 1;
#else
  if ((brg & 0x03L) >= 2) u32_brg = u32_brg/4;
  else u32_brg = u32_brg/4 - 1;
#endif
  ASSERT(u32_brg < 65536);
  U4BRG = u32_brg;
#else
#if (DEFAULT_BRGH4 == 0)
  f_brg = (((float) FCY)/((float) u32_baudRate)/16.0) - 1.0;
#else
  f_brg = (((float) FCY)/((float) u32_baudRate)/4.0) - 1.0;
#endif
  ASSERT(f_brg < 65535.5);
  U4BRG = roundFloatToUint16(f_brg);
#endif

  // Set up the UART mode register
  U4MODE =
    (1u   << 15) |      // UARTEN = 1 (enable the UART)
    (0u   << 13) |      // USIDL = 0  (continue operation in idle mode)
    (0u   << 12) |      // IREN = 0   (IrDA encoder and decoder disabled)
    (0u   << 11) |      // RTSMD = 0  (UxRTS# in flow control mode. Given
                        //  the UEN setting below, this doesn't matter.)
    (0b00 <<  8) |      // UEN = 00   (UxTX and UxRx pins are enabled and used;
                        //  used; UxCTS, UxRTS, and BCLKx pins are
                        //  controlled by port latches)
    (0u   <<  7) |      // WAKE = 0   (Wake-up on start bit detect during
                        //  sleep mode disabled)
    (0u   <<  6) |      // LPBACK = 0 (UARTx loopback mode is disabled)
    (0u   <<  5) |      // ABAUD = 0  (Auto-baud disabled)
    (0u   <<  4) |      // URXINV = 0 (Receve polarity inversion bit:
                        //  UxRX idle state is 1)
    (DEFAULT_BRGH4 << 3) | // BRGH (High/low baud rate select bit:
                        //  1 = high speed, 0 = low speed)
    (0b00 <<  1) |      // PDSEL = 00 (8-bit data, no parity)
    (0u   <<  0);       // STSEL = 0  (1 stop bit)

  // Set up the UART status and control register
  U4STA =
    (0u   << 15) |      // UTXISEL1 = 0  (See bit 13 below for explanation)
    (0u   << 14) |      // UTXINV = 0    (UxTX idle state is 1 (though docs
                        //  say 0))
    (0u   << 13) |      // UTXISEL0 = 0  (With bit 15 above, UTXISEL = 00:
                        //  Interrupt generated when any character
                        //  is transferred to the Transmit Shift Register).
    (0u   << 11) |      // UTXBRK = 0    (Sync break transmission disabled)
    (1u   << 10) |      // UTXEN = 0     (UARTx transmitter enabled. NOTE: per
                        //  the data sheet, this must be set *AFTER* UARTEN
                        //  is set to 1 (see UxMODE assignment above).
    (0b00 <<  6) |      // URXISEL = 00  (Interrupt flag bit is set when a
                        //  character is received)
    (0u   <<  5) |      // ADDEN = 0     (Address detect mode disabled)
    (0u   <<  1);       // OERR = 0      (Clear any overrun errors)

  // Delay 1 ms (at least one bit time) before beginning transmission,
  // since the UxTX pin is initially configured as an input on device
  // reset. It needs to be high for at least one bit time before
  // a character is sent in order for the start bit to be recognized.
  // See 24H FRM Sec. 17.5 for more details.
  ASSERT(u32_baudRate >= 1000L);
  // Note: If the baud rate is less than 1000, this delay
  // isn't long encough. The alternative is to use floating-point
  // math to compute the correct delay, which seems worse than the
  // problem.
  DELAY_MS(1);

  // Clear any errors
  U4STAbits.OERR = 0;
  while (U4STAbits.PERR || U4STAbits.FERR) {
    U4RXREG; //clear errors by reading RXREG
  }

  // Set up interrupts if requested
#ifdef UART4_RX_INTERRUPT
  _U4RXIF = 0;            //clear the flag
  _U4RXIP = UART4_RX_INTERRUPT_PRIORITY; //choose a priority
  _U4RXIE = 1;            //enable the interrupt
#endif
#ifdef UART4_TX_INTERRUPT
  //do not clear the U4TXIF flag!
  _U4TXIP = UART4_TX_INTERRUPT_PRIORITY; //choose a priority
  //do not enable the interrupt until we try to write to the UART
#endif

}

#endif // #if (NUM_UARTS >= 4)




