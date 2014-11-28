/*
 * rn4020.c
 * RN4020 Bluetooth LE Module control and interface logic
 */

#include "rn4020.h"
#include "ioport.h"
#include "timer.h"
#include <string.h>
#include "system_log.h"
#include <xc.h>
#include "bus_slave.h"

rn4020_info bt_data;

//Internal functions that should not called outside of this module
static BluetoothResult	bt_cmd_sync(const char *cmd);
static BluetoothResult 	bt_rcv_sync();
static void 			bt_start_transmission();
static void 			bt_prepare_rcv_buffer();

static BluetoothResult 	bt_enable_module();
static BluetoothResult 	bt_disable_module();


void bt_init()
{
	//Setup pin directions for control signals between RN4020 and PIC
	LAT(BT_SOFTWAKEPIN) = 0;
	DIR(BT_SOFTWAKEPIN) = OUTPUT;
#if TYPE_R(BT_SOFTWAKEPIN)
	TYPE(BT_SOFTWAKEPIN) = DIGITAL;
#endif

	LAT(BT_HARDWAKEPIN) = 0;
	DIR(BT_HARDWAKEPIN) = OUTPUT;
#if TYPE_R(BT_HARDWAKEPIN)
	TYPE(BT_HARDWAKEPIN) = DIGITAL;
#endif

	LAT(BT_CMDPIN) = 0;
	DIR(BT_CMDPIN) = OUTPUT;
#if TYPE_R(BT_CMDPIN)
	TYPE(BT_CMDPIN) = DIGITAL;
#endif

	//Setup the reprogrammable pins
	MAP_PERIPHERAL_IN(BT_RXRP, BT_RX_NAME);
	MAP_PERIPHERAL_IN(BT_CTSRP, BT_CTS_NAME);
	MAP_PERIPHERAL_OUT(BT_RTSRP, BT_RTS_NAME);
	MAP_PERIPHERAL_OUT(BT_TXRP, BT_TX_NAME);
	
	//Configure the UART
	BT_UMODE.RTSMD = 0; //Flow control mode for RT
	BT_UMODE.WAKE = 0;
	BT_UMODE.LPBACK = 0;
	BT_UMODE.IREN = 0;
	BT_UMODE.USIDL = 0; //Function in idle mode
	BT_UMODE.ABAUD = 0;
	BT_UMODE.RXINV = 0;
	BT_USTA.UTXINV = 0;
	BT_USTA.UTXISEL1 = 1;
	BT_USTA.UTXISEL0 = 0; //Transmit interrupt when buffer if empty
	BT_USTA.URXISEL = 0b00; //Receive interrupt on every character

	//Configure speed and parity (spec is 115200 baud, 8n1)
	BT_UMODE.PDSEL = 0x00; //8-bits no parity
	BT_UMODE.STSEL = 0; //1 stop bit

	BT_UMODE.BRGH = 1; //High speed
	BT_UBRG = 34; //actual speed 114285 baud 0.8% low

	//Initialize the buffer information
	bt_data.send_cursor = 0;
	bt_data.receive_cursor = 0;
	bt_data.flags_value = 0;

	//Clear receive buffer
    while(BT_USTA.URXDA == 1)
       BT_URX;

	//Setup interrupt information
	BT_RX_IF = 0;
	BT_TX_IF = 0;
	BT_RX_IP = 0b100;
	BT_TX_IP = 0b010;
	BT_RX_IE = 1;
	BT_TX_IE = 1;

	//Setup timeout timer information
	timer_load_period(BT_TIMER, RN4020_TIMEROUT_PERIOD);
	timer_function_idle(BT_TIMER, 1);
	timer_set_prescaler(BT_TIMER, kTimerPrescale256);
	timer_int_priority(BT_TIMER) = 0b100;
	timer_int_flag(BT_TIMER) = 0;
	timer_int_enable(BT_TIMER) = 1;

	//Enable module
	BT_UMODE.UARTEN = 1;
	BT_USTA.UTXEN = 1;

	

	//Make sure we can talk to the module
	if (bt_enable_module() == kBT_NoError)
	{
		bt_data.flags.initialized = 1;
		bt_disable_module();
	}
}

//FIXME: Change interrupt name to match BT UART name
void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
	while(BT_USTA.URXDA == 1)
	{
		if (bt_data.receive_cursor >= MAX_RN4020_MSG_SIZE)
		{
			bt_data.flags.receive_overflow = 1;
			//FIXME: Log an error here
		}
		else
		{
			unsigned char data = BT_URX;

			//Check if we've received the end of a command
			if (data == '\n' && bt_data.receive_buffer[bt_data.receive_cursor-1] == '\r')
			{
				bt_data.receive_buffer[bt_data.receive_cursor-1] = '\0';
				bt_data.flags.resp_received = 1;

				if (!bt_data.flags.cmd_sync)
					; //FIXME: schedule callback to handle event if it's not synchronous
			}
			else
				bt_data.receive_buffer[bt_data.receive_cursor++] = data;
		}
	}

	BT_RX_IF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
	while ( BT_USTA.UTXBF == 0 && bt_data.transmitted_cursor != bt_data.send_cursor) 
		BT_UTX = bt_data.send_buffer[bt_data.transmitted_cursor++];

	BT_TX_IF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _T1Interrupt()
{
	bt_data.flags.timeout = 1;
	timer_int_flag(BT_TIMER) = 0;
}

BluetoothResult bt_cmd_sync(const char *cmd)
{
	BluetoothResult result;

	bt_data.send_cursor = 0;

	while(cmd[bt_data.send_cursor] != 0)
	{
		if (bt_data.send_cursor >= MAX_RN4020_MSG_SIZE)
			return kBT_SendOverflow;

		bt_data.send_buffer[bt_data.send_cursor] = cmd[bt_data.send_cursor];
		++bt_data.send_cursor;
	}

	//Make sure there's room for the \n
	if (bt_data.send_cursor >= MAX_RN4020_MSG_SIZE)
		return kBT_SendOverflow;

	bt_data.send_buffer[bt_data.send_cursor++] = '\n';

	//Send the command and wait for a response
	bt_data.flags.cmd_sync = 1;
	bt_start_transmission();

	result = bt_rcv_sync();
	if (result != kBT_NoError)
		return result;

	return kBT_NoError;
}

BluetoothResult bt_rcv_sync()
{
	bt_data.flags.timeout = 0;
	timer_clear(BT_TIMER);
	timer_start(BT_TIMER);

	while(!bt_data.flags.resp_received && !bt_data.flags.timeout)
		;

	timer_stop(BT_TIMER);

	bt_data.flags.resp_received = 0;

	if (bt_data.flags.timeout)
		return kBT_Timeout;

	return kBT_NoError;
}

BluetoothResult bt_enable_module()
{
	BluetoothResult result;

	if (bt_data.flags.awake)
		return kBT_NoError;

	bt_prepare_rcv_buffer();
	LAT(BT_SOFTWAKEPIN) = 1;
	result = bt_rcv_sync();

	if (result != kBT_NoError)
	{
		DEBUG_LOGL("Timeout receiving response from BTLE module.");
		return result;
	}

	if (strcmp("CMD", bt_data.receive_buffer) != 0)
	{
		DEBUG_LOGL("Could not initialize RN4020 BTLE Module.");
		return kBT_InvalidResponse;
	}

	bt_data.flags.awake = 1;
	DEBUG_LOGL("Properly initialized RN4020 BTLE Module.");
	return kBT_NoError;
}

BluetoothResult bt_disable_module()
{
	BluetoothResult result;

	if (!bt_data.flags.awake)
		return kBT_NoError;

	bt_prepare_rcv_buffer();
	LAT(BT_SOFTWAKEPIN) = 0;
	result = bt_rcv_sync();

	if (result != kBT_NoError)
	{
		DEBUG_LOGL("Timeout receiving response from BTLE module.");
		return result;
	}

	if (strcmp("END", bt_data.receive_buffer) != 0)
	{
		DEBUG_LOGL("Could not turn off RN4020 BTLE Module.");
		return kBT_InvalidResponse;
	}

	bt_data.flags.awake = 0;
	return kBT_NoError;
}

void bt_prepare_rcv_buffer()
{
	bt_data.flags.resp_received = 0;
	bt_data.receive_cursor = 0;
}

void bt_start_transmission()
{
	bt_data.transmitted_cursor = 1;
	bt_prepare_rcv_buffer();

	BT_UTX = bt_data.send_buffer[0];
}

void bt_debug_buffer()
{
	bus_slave_return_buffer(bt_data.receive_buffer, 20);
}