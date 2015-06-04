/*
 * rn4020.h
 * This module handles serial communication with the Microchip RN4020 Bluetooth
 * Low Energy module.  It requires that the appropriate pins be configured using
 * definitions as follows:
 * 
 * Pin 					Defined Name
 * Tx PIC->RN4020 		BT_TXRP
 * Rx PIC->RN4020 		BT_RXRP
 * CTS PIC->RN4020 		BT_CTSRP
 * RTS PIC->RN4020 		BT_RTSRP
 * WAKE_SW 				BT_SOFTWAKEPIN
 * WAKE_HW				BT_HARDWAKEPIN
 * CMD/MLDP 			BT_CMDPIN
 * UART Number to use 	BT_UART 		1 to 4
 */

#ifndef __rn4020_h__
#define __rn4020_h__

#include <stdint.h>

#define MAX_RN4020_MSG_SIZE		100
#define RN4020_TIMEROUT_PERIOD	800 		//50 ms on an 8mhz crystal
#define RN4020_300ms			4800		//300 ms on an 8mhz crystal FIXME: shouldn't this be for a 32mhz crystal
#define RN4020_2s 				32000		//2s on an 8mhz crystal

#define build_register_r(prefix, middle, postfix)  	prefix ## middle ## postfix
#define build_register(prefix, middle, postfix)  	build_register_r(prefix, middle, postfix)

#define BT_UMODE 	build_register(U, BT_UART, MODEbits)
#define BT_USTA  	build_register(U, BT_UART, STAbits)
#define BT_URX   	build_register(U, BT_UART, RXREG)
#define BT_UTX	 	build_register(U, BT_UART, TXREG)
#define BT_UBRG  	build_register(U, BT_UART, BRG)

#define BT_RX_NAME 	build_register(UART, BT_UART, _RX)
#define BT_TX_NAME 	build_register(UART, BT_UART, _TX)
#define BT_CTS_NAME build_register(UART, BT_UART, _CTS)
#define BT_RTS_NAME build_register(UART, BT_UART, _RTS)

#define BT_RX_IF 	build_register(_U, BT_UART, RXIF)
#define BT_RX_IP 	build_register(_U, BT_UART, RXIP)
#define BT_RX_IE 	build_register(_U, BT_UART, RXIE)

#define BT_TX_IF 	build_register(_U, BT_UART, TXIF)
#define BT_TX_IP 	build_register(_U, BT_UART, TXIP)
#define BT_TX_IE 	build_register(_U, BT_UART, TXIE)

enum
{
	k38400Baud = 25, //NB This is using the standard 8 Mhz clock
	k115200Baud = 34 //NB This is using a 32 Mhz clock
};

typedef struct
{
	unsigned int initialized: 		1;
	unsigned int awake: 	  		1;
	unsigned int dormant: 	  		1;
	unsigned int resp_received:		1;
	unsigned int cmd_sync :			1;

	unsigned int timeout: 			1;
	unsigned int receive_overflow:	1; 
	unsigned int reserved: 	  		9;
} rn4020_flags;

typedef struct
{
	union 
	{
		volatile rn4020_flags flags;
		volatile unsigned int flags_value;
	};

	char send_buffer[MAX_RN4020_MSG_SIZE];
	char receive_buffer[MAX_RN4020_MSG_SIZE];

	unsigned int transmitted_cursor;
	unsigned int send_cursor;
	unsigned int receive_cursor;
} rn4020_info;

typedef enum
{
	kBT_NoError = 0,
	kBT_Timeout = 1,
	kBT_SendOverflow = 2,
	kBT_ReceiveOverflow = 3,
	kBT_InvalidResponse = 4,
	kBT_InvalidResponseLength = 5,
	kBT_ErrorResponseReceived = 6,
	kBT_InitializationError
} BluetoothResult;

typedef enum
{
	kBT_ParseResponse = 1 << 0,
	kBT_CommandPreloaded = 1 << 1
} BluetoothCommandFlags;

//Module API
BluetoothResult bt_init();
uint8_t 		bt_debug_buffer(uint8_t length);
BluetoothResult bt_advertise(unsigned int interval, unsigned int duration);
BluetoothResult bt_broadcast(const char *data, unsigned int length);
BluetoothResult bt_setname(const char *name);

//FIXME: Add error statements for all bt required pins
//Make sure all of the appropriate pins are defined
#ifndef BT_SOFTWAKEPIN
#error BT_SOFTWAKEPIN must be defined to use the bluetooth RN4020 module
#endif

#ifndef BT_HARDWAKEPIN
#error BT_HARDWAKEPIN must be defined to use the bluetooth RN4020 module
#endif

#ifndef BT_CMDPIN
#error BT_CMDPIN must be defined to use the bluetooth RN4020 module
#endif


#endif