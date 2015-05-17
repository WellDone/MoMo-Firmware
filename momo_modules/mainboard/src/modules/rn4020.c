/*
 * rn4020.c
 * RN4020 Bluetooth LE Module control and interface logic
 */

#include "rn4020.h"
#include "ioport.h"
#include "timer.h"
#include "oscillator.h"
#include <string.h>
#include "system_log.h"
#include <xc.h>
#include "bus_slave.h"
#include "log_definitions.h"

rn4020_info bt_data;

//Internal functions that should not called outside of this module
static BluetoothResult	bt_cmd_sync(const char *cmd, unsigned int flags);
static BluetoothResult 	bt_rcv_sync(unsigned int timeout);
static void 			bt_start_transmission();
static void 			bt_prepare_rcv_buffer();
static void 			bt_configure_uart(bool highspeed, unsigned int baud);
static BluetoothResult 	bt_reboot();
static BluetoothResult 	bt_enable_module();
static BluetoothResult 	bt_disable_module();


void bt_init()
{
	BluetoothResult err;

	//Setup pin directions for control signals between RN4020 and PIC
	LAT(BT_SOFTWAKEPIN) = 0;
	DIR(BT_SOFTWAKEPIN) = OUTPUT;
	ENSURE_DIGITAL(BT_SOFTWAKEPIN);

	LAT(BT_HARDWAKEPIN) = 0;
	DIR(BT_HARDWAKEPIN) = OUTPUT;
	ENSURE_DIGITAL(BT_HARDWAKEPIN);

	LAT(BT_CMDPIN) = 0;
	DIR(BT_CMDPIN) = OUTPUT;
	ENSURE_DIGITAL(BT_CMDPIN);

	//Setup the reprogrammable pins
	MAP_PERIPHERAL_IN(BT_RXRP, BT_RX_NAME);
	MAP_PERIPHERAL_IN(BT_CTSRP, BT_CTS_NAME);
	MAP_PERIPHERAL_OUT(BT_RTSRP, BT_RTS_NAME);
	MAP_PERIPHERAL_OUT(BT_TXRP, BT_TX_NAME);

	//Setup timeout timer information
	timer_load_period(BT_TIMER, RN4020_TIMEROUT_PERIOD);
	timer_function_idle(BT_TIMER, 1);
	timer_set_prescaler(BT_TIMER, kTimerPrescale256);
	timer_int_priority(BT_TIMER) = 0b100;
	timer_int_flag(BT_TIMER) = 0;
	timer_int_enable(BT_TIMER) = 1;

	//Initialize state
	bt_data.flags_value = 0;

	/*
	 * The BTLE module defaults (on factory config) to 115200 baud but we can't
	 * generate that within 2% error using an 8Mhz crystal, so we need to use 38400
	 * baud. However, if the BTLE module has not been configured yet, we temporarily
	 * increase our clockspeed to 32 Mhz so we can talk to the module and send it the
	 * command to change its baud rate.
	 */
	/*bt_configure_uart(true, k38400Baud);
	err = bt_enable_module();
	if (err != kBT_NoError)
	{
		//If we couldn't talk to the module at 38400, try at 115200 and attempt to set the 
		//baud rate down to 38400
		bool config_successful = false;
		
		LOG_DEBUG(kBTCouldNotCommunicate);
		LOG_INT(err);

		//Try talking at 115200 baud (we need to use a 32Mhz oscillator to get the baud error low enough)
		//set_oscillator_speed(k8MhzFRC, true);
		bt_configure_uart(true, k115200Baud);
		err = bt_enable_module();
		if (err != kBT_NoError)
		{
			LOG_DEBUG(kBTCouldNotTalkAt115200);
			LOG_INT(err);
		}
		else
		{
			//Set the baud rate down to 38200
			err = bt_cmd_sync("SB,3", kBT_ParseResponse);
			if (err == kBT_NoError)
			{
				bt_reboot();

				//set_oscillator_speed(k8MhzFRC, false);
				bt_configure_uart(true, k38400Baud);

				bt_prepare_rcv_buffer();
				err = bt_rcv_sync(RN4020_300ms); //CHECK: may need to up the timeout here since reboots are slow
				if (err == kBT_NoError && (strcmp(bt_data.receive_buffer, "CMD") == 0))
					config_successful = true;
			}
		}

		//Make sure we always set the oscillator back to the correct setting
		//set_oscillator_speed(k8MhzFRC, false);

		if (config_successful)
			LOG_DEBUG(kBTResetBaud);
		else
		{
			LOG_DEBUG(kBTCouldNotResetBaud);
			LOG_INT(err);
			LOG_ARRAY(bt_data.receive_buffer, bt_data.receive_cursor);
		}
		
	}*/
	
	//bt_disable_module();

	//Make sure we can talk to the module now (in case we had to reset the baud rate above)
	/*if (bt_enable_module() == kBT_NoError)
	{
		bt_data.flags.initialized = 1;
		bt_disable_module();

		LOG_CRITICAL(kBTModuleInitializedCorrectly);

		/*
		if (bt_advertise() != kBT_NoError)
			LOG_DEBUG(kCouldNotAdvertise);
		else
			LOG_DEBUG(kBTAdvertisementSuccessful);
		*/
	//}

	err = bt_enable_module();
	LOG_DEBUG(kBTCouldNotTalkAt115200);
	LOG_INT(err);
	flush_log(NULL);
	//bt_disable_module();
}

void bt_configure_uart(bool highspeed, unsigned int baud)
{
	//Disable module
	BT_UMODE.UARTEN = 0;
	BT_USTA.UTXEN = 0;

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

	BT_UMODE.BRGH = highspeed? 1 : 0;
	BT_UBRG = baud; 

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

	//Enable module
	BT_UMODE.UARTEN = 1;
	BT_USTA.UTXEN = 1;
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
				bt_data.receive_buffer[--bt_data.receive_cursor] = '\0';
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

BluetoothResult bt_cmd_sync(const char *cmd, unsigned int flags)
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

	result = bt_rcv_sync(0);
	if (result != kBT_NoError)
	{
		LOG_DEBUG(kBTErrorSendingCommand);
		LOG_ARRAY(bt_data.send_buffer, bt_data.send_cursor);
		LOG_ARRAY(bt_data.receive_buffer, bt_data.receive_cursor);
		return result;
	}

	//Check if we should parse the response as AOK or ERR
	if (!(flags & kBT_ParseResponse))
		return kBT_NoError;

	//Valid responses to commands should be AOK or ERR so check to
	//see what we got.
	if (bt_data.receive_cursor != 3)
		return kBT_InvalidResponseLength;

	if (bt_data.receive_buffer[0] == 'A' && bt_data.receive_buffer[1] == 'O' && bt_data.receive_buffer[2] == 'K')
		return kBT_NoError;

	if (bt_data.receive_buffer[0] == 'E' && bt_data.receive_buffer[1] == 'R' && bt_data.receive_buffer[2] == 'R')
		return kBT_ErrorResponseReceived;

	return kBT_InvalidResponse;
}

BluetoothResult bt_rcv_sync(unsigned int timeout)
{
	bt_data.flags.timeout = 0;

	//Allow custom timeouts if needed
	if (timeout != 0)
		timer_load_period(BT_TIMER, timeout);

	timer_clear(BT_TIMER);
	timer_start(BT_TIMER);

	while(!bt_data.flags.resp_received && !bt_data.flags.timeout)
		;

	timer_stop(BT_TIMER);
	timer_load_period(BT_TIMER, RN4020_TIMEROUT_PERIOD);

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
	result = bt_rcv_sync(0);

	if (result != kBT_NoError)
	{
		LOG_CRITICAL(kBTTimeout);
		LAT(BT_SOFTWAKEPIN) = 0;
		return result;
	}

	if (strcmp("CMD", bt_data.receive_buffer) != 0)
	{
		LOG_CRITICAL(kBTNoWakeup);
		return kBT_InvalidResponse;
	}

	bt_data.flags.awake = 1;
	return kBT_NoError;
}

BluetoothResult bt_disable_module()
{
	BluetoothResult result;

	if (!bt_data.flags.awake)
		return kBT_NoError;

	bt_prepare_rcv_buffer();
	LAT(BT_SOFTWAKEPIN) = 0;
	result = bt_rcv_sync(0);

	if (result != kBT_NoError)
	{
		LOG_CRITICAL(kBTTimeout);
		return result;
	}

	if (strcmp("END", bt_data.receive_buffer) != 0)
	{
		LOG_CRITICAL(kBTNoShutdown);
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

uint8_t bt_debug_buffer(uint8_t length)
{
	bus_slave_return_buffer(bt_data.receive_buffer, 20);

	return kNoErrorStatus;
}

//FIXME: Finish this function
void bt_encode(unsigned int number, char *output)
{
	int i;

	for (i=0; i<4; ++i)
	{
		char hex;
		unsigned int val = (number >> (i*4)) & 0xF;
	}
}

BluetoothResult bt_reboot()
{
	BluetoothResult err;

	err = bt_cmd_sync("R,1", 0);
	if (err != kBT_NoError)
		return err;

	if (strcmp(bt_data.receive_buffer, "Reboot") != 0)
		return kBT_InvalidResponse;

	return kBT_NoError;
}

BluetoothResult bt_advertise()
{
	BluetoothResult result;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	result = bt_cmd_sync("A", kBT_ParseResponse);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	result = bt_disable_module();
	return result;
}