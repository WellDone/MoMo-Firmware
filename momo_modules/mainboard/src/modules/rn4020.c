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
#include <string.h>
#include "bus_slave.h"
#include "log_definitions.h"
#include "mib_definitions.h"
#include "memory_manager.h"
#include "flashblock.h"
#include "base64.h"

#include "scheduler.h"
#include "common.h"
#include "task_manager.h"
#include "bus_master.h"

static rn4020_info 	 bt_data;
static rn4020_persistant_state rn4020_state;
static flash_block_info rn4020_fb;

//Internal functions that should not called outside of this module
static BluetoothResult 	bt_cmd_sync(const char *cmd, unsigned int flags, unsigned int timeout);
static BluetoothResult  bt_rcv_sync(unsigned int timeout, unsigned int flags);
static void 			bt_start_transmission();
static void 			bt_prepare_rcv_buffer();
static void 			bt_configure_uart(bool highspeed, unsigned int baud);
static BluetoothResult 	bt_reboot(int sync);
static BluetoothResult 	bt_enable_module();
static BluetoothResult 	bt_disable_module();

static void 			bt_encode(uint8_t input, char *output);
static char 			bt_encode_nibble(uint8_t nibble);

static uint32_t 		bt_decode_uint32(char *data);
static uint8_t 			bt_decode_nibble(char *data);
static uint8_t 			bt_decode_byte(char *data);

static void 			bt_process_mib_packet(void *arg);
static void 			bt_return_mib_response(uint8_t status, void *state);

static BluetoothResult 	bt_setupservices();
static BluetoothResult 	bt_load_script();

static void 			bt_configure_automode();

static void				bt_close_connection(void *arg);
static void 			bt_open_connection(void *arg);

static BluetoothResult 	bt_send_mldp(char *data, unsigned int length);

BluetoothResult bt_init()
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

	DIR(BT_MLDP_EV) = INPUT;
	ENSURE_DIGITAL(BT_MLDP_EV);

	DIR(D9) = OUTPUT;

	//Setup the reprogrammable pins
	MAP_PERIPHERAL_IN(BT_RXRP, BT_RX_NAME);
	MAP_PERIPHERAL_IN(BT_CTSRP, BT_CTS_NAME);
	MAP_PERIPHERAL_OUT(BT_RTSRP, BT_RTS_NAME);
	MAP_PERIPHERAL_OUT(BT_TXRP, BT_TX_NAME);

	//Setup timeout timer information
	timer_function_idle(BT_TIMER, 1);
	timer_set_prescaler(BT_TIMER, kTimerPrescale256);
	timer_int_priority(BT_TIMER) = 0b100;
	timer_int_flag(BT_TIMER) = 0;
	timer_int_enable(BT_TIMER) = 1;

	//Initialize state
	bt_data.flags_value = 0;
	bt_data.checksum_errors = 0;

	//Check our state
	fb_init(&rn4020_fb, kRN4020ConfigSubsector, sizeof(rn4020_persistant_state), kRN4020StructureVersion);
	if (fb_count(&rn4020_fb) > 0)
		fb_read(&rn4020_fb, &rn4020_state);
	else
	{
		rn4020_state.state = kScriptNotLoaded;
		fb_write(&rn4020_fb, &rn4020_state);
	}

	/*
	 * The BTLE module defaults (on factory config) to 115200 baud but we can't
	 * generate that within 2% error using an 8Mhz crystal, so we need to use 38400
	 * baud. However, if the BTLE module has not been configured yet, we temporarily
	 * increase our clockspeed to 32 Mhz so we can talk to the module and send it the
	 * command to change its baud rate.
	 */

	bt_configure_uart(true, k38400Baud);

	//In case this is a hardware powerup, wait for the UART RX line to go high indicating the RN4020
	//has finished powering up.  This should take < 2s.
	timer_load_period(BT_TIMER, RN4020_2s);

	bt_data.flags.timeout = 0;

	timer_clear(BT_TIMER);
	timer_start(BT_TIMER);

	while (!bt_data.flags.timeout && !PIN(BT_TXPIN))
		;

	timer_stop(BT_TIMER);

	//Give the module time to initialize itself
	DELAY_MS(10);

	timer_load_period(BT_TIMER, RN4020_TIMEROUT_PERIOD);

	if (rn4020_state.state == kScriptNotLoaded)
	{
		err = bt_enable_module();
		if (err != kBT_NoError)
		{
			
			//If we couldn't talk to the module at 38400, try at 115200 and attempt to set the 
			//baud rate down to 38400
			bool config_successful = false;
			
			LOG_DEBUG(kBTCouldNotCommunicate);
			LOG_INT(err);

			//Try talking at 115200 baud (we need to use a 32Mhz oscillator to get the baud error low enough)
			set_oscillator_speed(k8MhzFRC, true);
			DELAY_US(128*4);

			bt_configure_uart(true, k115200Baud);

			err = bt_enable_module();
			if (err != kBT_NoError)
			{
				LOG_DEBUG(kBTCouldNotTalkAt115200);
			}
			else
			{
				//Set the baud rate down to 38400
				err = bt_cmd_sync("SB,3", kBT_ParseResponse, RN4020_TIMEROUT_PERIOD*4);
				if (err == kBT_NoError)
				{
					bt_reboot(0);

					set_oscillator_speed(k8MhzFRC, false);
					bt_configure_uart(true, k38400Baud);

					//Wait for tx pin to go low and high indicating that we've rebooted
					while (PIN(BT_TXPIN))
						;

					while (!PIN(BT_TXPIN))
						;

					bt_prepare_rcv_buffer();
					err = bt_rcv_sync(RN4020_2s, 0);
					if (err == kBT_NoError && (strcmp(bt_data.receive_buffer, "CMD") == 0))
						config_successful = true;
				}
			}

			//Make sure we always set the oscillator back to the correct setting
			set_oscillator_speed(k8MhzFRC, false);

			if (config_successful)
				LOG_DEBUG(kBTResetBaud);
			else
			{
				LOG_DEBUG(kBTCouldNotResetBaud);
				LOG_INT(err);
				LOG_INT(bt_data.receive_cursor);
				LOG_ARRAY(bt_data.receive_buffer, bt_data.receive_cursor);
			}
			
		}
		
		bt_disable_module();

		/*
		 * Make sure we can talk to the module now (in case we had to reset the baud rate above)
		 * 
		 * Also begin the process of setting the module up to have the correct services so that 
		 * we can send MIB commands over BTLE.  If everything is successful, load in the automatic
		 * configuration script onto the RN4020 so that it sets itself up correctly.  This will not
		 * take effect until the device is next power cycled but it is a one-time setup event.
		 */

		if (bt_enable_module() == kBT_NoError)
		{
			bt_data.flags.initialized = 1;
			bt_disable_module();

			err = bt_setupservices();
			if (err == kBT_NoError)
			{
				err = bt_load_script();
	       
	        	if (err == kBT_NoError)
	        	{
	            	LOG_CRITICAL(kBTLoadedScript);
	            	
	        		//Now configure the module for autonomous mode
	            	err = bt_setfeatures(kRN4020AutonomousConfig);
	            	if (err == kBT_NoError)
	            	{
	            		LOG_CRITICAL(kBTModuleInAutonomousMode);
	            		rn4020_state.state = kScriptLoaded;
	            		fb_write(&rn4020_fb, &rn4020_state);
	            	}
	            }
	        }

			return err;
		}

		return kBT_InitializationError;
	}
	else
	{
		//The script has already been loaded
		bt_configure_automode();
		LOG_CRITICAL(kBTModuleRunningInAutonomousMode);
		return kBT_NoError;
	}
}

void bt_configure_automode()
{
	//Setup CN on connection pin so that we know when there's a connection pending
	//Pin F6, CN84
	DIR(F6) = INPUT;
	_CN84IE = 1;
	_CNIF = 0;
	_CNIP = 0b010;
	_CNIE = 1;

	BT_RX_IE = 0;

	if (PIN(F6))
		bt_open_connection(NULL);
}

BluetoothResult bt_load_script()
{
	BluetoothResult result;
	const char *script = "@PW_ON\nA,07D0\n@DISCON\nA,07D0\n\x1b";
	unsigned int script_length = strlen(script);

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	result = bt_cmd_sync("WP", kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	result = bt_cmd_sync("WC", kBT_ParseResponse, RN4020_2s);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	result = bt_cmd_sync("WW", kBT_ParseResponse, RN4020_2s);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	//Now enter the script
	strcpy(bt_data.send_buffer, script);
	bt_data.send_cursor = script_length;
	bt_data.flags.cmd_sync = 1;
	bt_start_transmission();

	/*
	 * The module responds to the command by sending \r\nEND\r\n
	 * so we need to receive 2 separate responses.
	 */
	result = bt_rcv_sync(RN4020_2s, kBT_ContinueWaiting);
	if (result != kBT_NoError)
	{
		LOG_DEBUG(kBTReceivedUnknownMessage);
		bt_data.receive_buffer[bt_data.receive_cursor] = '\0';
		LOG_STRING(bt_data.receive_buffer);
		bt_disable_module();
		return result;
	}

	result = bt_rcv_sync(RN4020_2s, 0);
	if (result != kBT_NoError)
	{
		LOG_DEBUG(kBTReceivedUnknownMessage);
		bt_data.receive_buffer[bt_data.receive_cursor] = '\0';
		LOG_STRING(bt_data.receive_buffer);
		bt_disable_module();
		return result;
	}

	if (strcmp("END", bt_data.receive_buffer) != 0)
	{
		LOG_DEBUG(kBTReceivedUnknownMessage);
		bt_data.receive_buffer[bt_data.receive_cursor] = '\0';
		LOG_STRING(bt_data.receive_buffer);
		bt_disable_module();
		return kBT_InvalidResponse;
	}

	return bt_disable_module();
}

BluetoothResult bt_setupservices()
{
	BluetoothResult err;
	uint32_t services;

	err = bt_readservices(&services);
	if (err != kBT_NoError)
		return err;

	//Make sure the device information and private services are available
	if (services != 0x80000001ULL)
	{
		err = bt_setservices(0x80000001ULL);
		if (err != kBT_NoError)
			return err;
	}

	return kBT_NoError;
}

uint32_t bt_decode_uint32(char *data)
{
	uint32_t retval = 0;

	retval |= ((uint32_t)bt_decode_byte(data+0)) << 24;
	retval |= ((uint32_t)bt_decode_byte(data+2)) << 16;
	retval |= ((uint32_t)bt_decode_byte(data+4)) << 8;
	retval |= ((uint32_t)bt_decode_byte(data+6)) << 0;

	return retval;

}

uint8_t bt_decode_nibble(char *data)
{
	if (data[0] >= '0' && data[0] <= '9')
		return data[0] - '0';

	return 10 + (data[0] - 'A');
}

uint8_t bt_decode_byte(char *data)
{
	return (bt_decode_nibble(data) << 4) | bt_decode_nibble(data+1);
}

BluetoothResult bt_readservices(uint32_t *out)
{
	BluetoothResult result;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	result = bt_cmd_sync("GS", 0, 0);
	if (result == kBT_NoError)
	{
		*out = bt_decode_uint32(bt_data.receive_buffer);
		return bt_disable_module();
	}
	
	bt_disable_module();
	return result;
}

BluetoothResult bt_readfeatures(uint32_t *out)
{
	BluetoothResult result;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	result = bt_cmd_sync("GR", 0, 0);
	if (result == kBT_NoError)
	{
		*out = bt_decode_uint32(bt_data.receive_buffer);
		return bt_disable_module();
	}
	
	bt_disable_module();
	return result;
}

BluetoothResult bt_readname(char *out, unsigned int length)
{
	BluetoothResult result;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	result = bt_cmd_sync("GN", 0, 0);
	if (result == kBT_NoError)
	{
		strncpy(out, bt_data.receive_buffer, length);
		return bt_disable_module();
	}
	
	bt_disable_module();
	return result;
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
	BT_USTA.UTXISEL1 = 0;
	BT_USTA.UTXISEL0 = 1; //Transmit interrupt when shift is done
	BT_USTA.URXISEL = 0b00; //Receive interrupt on every character

	//Configure speed and parity (spec is 115200 baud, 8n1)
	BT_UMODE.PDSEL = 0x00; //8-bits no parity
	BT_UMODE.STSEL = 0; //1 stop bit

	BT_UMODE.BRGH = highspeed? 1 : 0;
	BT_UMODE.UEN = 0b10;
	BT_UBRG = baud; 

	//Initialize the buffer information
	bt_data.send_cursor = 0;
	bt_data.receive_cursor = 0;
	bt_data.flags_value = 0;

	BT_RX_IE = 0;
	BT_TX_IE = 0;

	//Enable module
	BT_UMODE.UARTEN = 1;
	BT_USTA.UTXEN = 1;

	//Clear receive buffer
    while(BT_USTA.URXDA == 1)
       BT_URX;

	//Setup interrupt information
	BT_RX_IF = 0;
	BT_TX_IF = 0;
	BT_RX_IP = 0b111;
	BT_TX_IP = 0b010;
	BT_RX_IE = 1;
	BT_TX_IE = 1;
}

void bt_process_mib_packet(void *arg)
{
	char *packet = (char *)arg;
	MIBUnified mib_packet;
	uint8_t i;
	uint8_t sum = 0;

	//Get rid of the '@' prepended to the packet (keep a copy for debugging purposes)
	//Copy the address + packet + checksum
	memcpy(bt_data.cmd_payload, packet+1, kMIBMessageSize+1);

	//Verify the checksum
	for (i=0; i<kMIBMessageSize+1; ++i)
		sum += bt_data.cmd_payload[i];

	if (sum != 0)
		++bt_data.checksum_errors;

	memcpy((char*)&mib_packet.packet, bt_data.cmd_payload+1, kMIBMessageSize);
	mib_packet.address = (uint8_t)bt_data.cmd_payload[0];

	bus_master_rpc_async(bt_return_mib_response, &mib_packet, NULL);
}

void bt_return_mib_response(uint8_t status, void *state)
{
	char *packet = (char *)&mib_unified.packet;

	base64_encode((unsigned char *)packet, kMIBMessageSize, bt_data.mib_response_buffer+1, 36);

	//Add back in the @<data>!\r\n framing
	bt_data.mib_response_buffer[0] = '@';
	bt_data.mib_response_buffer[37] = '!';
	bt_data.mib_response_buffer[38] = '\r';
	bt_data.mib_response_buffer[39] = '\n';
	bt_send_mldp(bt_data.mib_response_buffer, 40);
	
	DMACH0 = 0;
	DMASRC0 = (unsigned int)&BT_URX;
	DMADST0 = (unsigned int)bt_data.receive_buffer;
	DMACNT0 = 1;

	DMACH0bits.SAMODE = 0; //Fixed source address
	DMACH0bits.DAMODE = 1; //Increment destination
	DMACH0bits.TRMODE = 1; //Repeated one-shot
	DMACH0bits.BYTE = 1;
	DMACH0bits.RELOAD = 0; //don't reload source and dest
	
	DMAINT0bits.CHSEL = 0b110001; //UART1 Receive Interrupt
	DMAINT0bits.HALFEN = 0;

	if (BT_USTA.OERR)
		BT_USTA.OERR = 0;

	//Clear out the receive buffer
	if (BT_USTA.OERR)
		BT_USTA.OERR = 0;

	while(BT_USTA.URXDA == 1)
		BT_URX;

	DMACH0bits.CHEN = 1;
}

void bt_open_connection(void *arg)
{
	LOG_DEBUG(kBTReceivedConnection);

	LAT(BT_CMDPIN) = 1;
	LAT(BT_SOFTWAKEPIN) = 1;

	DMACONbits.DMAEN = 1;
	DMACONbits.PRSSEL = 1;

	DMAL = (unsigned int)bt_data.receive_buffer;
	DMAH = (unsigned int)&bt_data.receive_buffer[MAX_RN4020_MSG_SIZE-1];
	
	DMACH0 = 0;
	DMASRC0 = (unsigned int)&BT_URX;
	DMADST0 = (unsigned int)bt_data.receive_buffer;
	DMACNT0 = 1;

	DMACH0bits.SAMODE = 0; //Fixed source address
	DMACH0bits.DAMODE = 1; //Increment destination
	DMACH0bits.TRMODE = 1; //Repeated one-shot
	DMACH0bits.BYTE = 1;
	DMACH0bits.RELOAD = 0; //don't reload source and dest
	
	DMAINT0bits.CHSEL = 0b110001; //UART1 Receive Interrupt
	DMAINT0bits.HALFEN = 0;


	_DMA0IF = 0;
	_DMA0IP = 0b111;

	//Clear out the receive buffer
	if (BT_USTA.OERR)
		BT_USTA.OERR = 0;

	while(BT_USTA.URXDA == 1)
		BT_URX;

	DMACH0bits.CHEN = 1;
	_DMA0IE = 1;

	taskloop_set_flag(kTaskLoopSleepBit, 0);
}

void bt_close_connection(void *arg)
{
	_DMA0IE = 0;
	DMACONbits.DMAEN = 0;	

	LAT(BT_SOFTWAKEPIN) = 0;

	LOG_DEBUG(kBTConnectionStopped);
	taskloop_set_flag(kTaskLoopSleepBit, 1);
}

void __attribute__((interrupt,no_auto_psv)) _DMA0Interrupt()
{
	char *last_data = (char *)(DMADST0 - 1);

	_DMA0IF = 0;

	if (*last_data == '!' && *(last_data - 27) == '@')
	{
		//Don't receive any more mib commands until we process this one
		DMACH0 = 0;
		taskloop_add(bt_process_mib_packet, last_data-27);
	}
}

void __attribute__((interrupt,no_auto_psv)) _CNInterrupt()
{
	//Read register to clear mismatch
	uint16_t value = PORTF;

	if (value & (1 << 6))
		taskloop_add(bt_open_connection, NULL);
	else
		taskloop_add(bt_close_connection, NULL);

	_CNIF = 0;
}

//FIXME: Change interrupt name to match BT UART name
void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
	if (bt_data.flags.ignore_received)
	{
		while(BT_USTA.URXDA == 1)
			BT_URX;
	}

	while(BT_USTA.URXDA == 1)
	{
		unsigned char data = BT_URX;

		if (bt_data.flags.waiting_for_resp)
		{
			if (bt_data.receive_cursor >= MAX_RN4020_RECEIVE_SIZE)
			{
				bt_data.flags.receive_overflow = 1;
				//FIXME: Log an error here
			}
			else
			{
				//Don't allow null bytes that can be caused by rebooting the RN4020
				if(data == 0)
					continue;

				//Check if we've received the end of a command
				if (bt_data.receive_cursor > 0 && data == '\n' && bt_data.receive_buffer[bt_data.receive_cursor-1] == '\r')
				{
					bt_data.receive_buffer[--bt_data.receive_cursor] = '\0';
					bt_data.flags.resp_received = 1;
				}
				else
					bt_data.receive_buffer[bt_data.receive_cursor++] = data;
			}
		}
	}

	BT_RX_IF = 0; //Clear IFS flag

	//If we had an overflow, clear the condition so that we receive more data.  The bytes have already been read from the buffer in the above code
	//so we won't lose data provided that the rn4020 actually abided by the RTS signal
	if (BT_USTA.OERR)
		BT_USTA.OERR = 0;
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
	if ( BT_USTA.UTXBF == 0 && bt_data.transmitted_cursor != bt_data.send_cursor)
	{
		BT_UTX = bt_data.send_buffer[bt_data.transmitted_cursor++];
	}

	BT_TX_IF = 0; //Clear IFS flag
}

//FIXME: Change to match BT_TIMER value
void __attribute__((interrupt,no_auto_psv)) _T4Interrupt()
{
	bt_data.flags.timeout = 1;
	timer_int_flag(BT_TIMER) = 0;
}

BluetoothResult bt_cmd_sync(const char *cmd, unsigned int flags, unsigned int timeout)
{
	BluetoothResult result;

	if (!(flags & kBT_CommandPreloaded))
	{
		bt_data.send_cursor = 0;
		while(cmd[bt_data.send_cursor] != 0)
		{
			if (bt_data.send_cursor >= MAX_RN4020_MSG_SIZE)
				return kBT_SendOverflow;

			bt_data.send_buffer[bt_data.send_cursor] = cmd[bt_data.send_cursor];
			++bt_data.send_cursor;
		}
	}

	//Make sure there's room for the \n
	if (bt_data.send_cursor >= MAX_RN4020_MSG_SIZE)
		return kBT_SendOverflow;

	bt_data.send_buffer[bt_data.send_cursor++] = '\n';

	//Send the command and wait for a response
	bt_data.flags.cmd_sync = 1;
	bt_start_transmission();

	result = bt_rcv_sync(timeout, flags);
	if (result != kBT_NoError)
	{
		LOG_DEBUG(kBTErrorSendingCommand);
		LOG_ARRAY(bt_data.send_buffer, bt_data.send_cursor);
		LOG_ARRAY(bt_data.receive_buffer, bt_data.receive_cursor);
		return result;
	}

	//Check if we should parse the response as AOK or ERR
	if (!(flags & kBT_ParseResponse))
	{
		DELAY_MS(10); //Give the device time to process the command
		return kBT_NoError;
	}

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

BluetoothResult bt_rcv_sync(unsigned int timeout, unsigned int flags)
{
	bt_data.flags.timeout = 0;
	bt_data.flags.waiting_for_resp = 1;

	//Allow custom timeouts if needed
	if (timeout != 0)
		timer_load_period(BT_TIMER, timeout);

	timer_clear(BT_TIMER);
	timer_start(BT_TIMER);

	while(!bt_data.flags.resp_received && !bt_data.flags.timeout)
		;

	if (!(flags & kBT_ContinueWaiting))
		bt_data.flags.waiting_for_resp = 0;

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

	result = bt_rcv_sync(0, 0);

	if (result != kBT_NoError)
	{
		LOG_CRITICAL(kBTTimeout);
		LAT(BT_SOFTWAKEPIN) = 0;
		DELAY_MS(5);				//Wait long enough for the rn4020 to clock out END if it is trying to
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
	result = bt_rcv_sync(0, 0);

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

BluetoothResult bt_send_mldp(char *data, unsigned int length)
{
	if (length > MAX_RN4020_MSG_SIZE)
		return kBT_SendOverflow;

	if (length == 0)
		return kBT_NoError;

	//We need to send 1 byte followed by a delay followed by the rest of the bytes since
	//there is an undocumented sleep feature of the rn4020 that needs time to wakeup
	memcpy(bt_data.send_buffer, data, length);
	bt_data.send_cursor = 1;
	bt_data.transmitted_cursor = 1;

	BT_UTX = bt_data.send_buffer[0];

	while (!BT_USTA.TRMT)
		;

	DELAY_MS(2);

	BT_UTX = bt_data.send_buffer[1];
	bt_data.transmitted_cursor = 2;
	bt_data.send_cursor = length;

	while (bt_data.transmitted_cursor != bt_data.send_cursor)
		;

	return kBT_NoError;
}

void bt_start_transmission()
{
	bt_data.transmitted_cursor = 1;
	bt_prepare_rcv_buffer();

	BT_UTX = bt_data.send_buffer[0];
}

uint8_t bt_debug_buffer(uint8_t length)
{

	bus_slave_return_buffer(((unsigned char *)&bt_data) + plist_get_int16(0), 20);
	return kNoErrorStatus;
}

//Encode an 8 bit number as hex.  Output must point to a char buffer
//with at least 2 bytes of space for the hex representation of the
//input.
void bt_encode(uint8_t input, char *output)
{
	uint8_t low, high;

	low = input & 0x0F;
	high = input >> 4;

	output[0] = bt_encode_nibble(high);
	output[1] = bt_encode_nibble(low);
}

char bt_encode_nibble(uint8_t nibble)
{
	nibble &= 0xF;
	if (nibble < 10)
		return '0' + nibble;

	nibble -= 10;

	return 'A' + nibble;
}

BluetoothResult bt_reboot(int sync)
{
	BluetoothResult err;

	err = bt_cmd_sync("R,1", 0, 0);
	if (err != kBT_NoError)
		return err;

	if (strcmp(bt_data.receive_buffer, "Reboot") != 0)
		return kBT_InvalidResponse;

	if (sync == 0)
		return kBT_NoError;

	bt_data.flags.ignore_received = 1;
	//Wait for tx pin to go low and high indicating that we've rebooted
	while (PIN(BT_TXPIN))
		;

	while (!PIN(BT_TXPIN))
		;

	bt_data.flags.ignore_received = 0;

	bt_prepare_rcv_buffer();
	err = bt_rcv_sync(RN4020_2s, 0);
	if (err == kBT_NoError && (strcmp(bt_data.receive_buffer, "CMD") == 0))
	{
		err = bt_disable_module();
		return err;
	}

	err = bt_disable_module();

	return kBT_Timeout;
}

BluetoothResult bt_advertise(unsigned int interval, unsigned int duration)
{
	BluetoothResult result;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	bt_data.send_buffer[0] = 'A';
	bt_data.send_buffer[1] = ',';
	bt_encode(interval >> 8, &bt_data.send_buffer[2]);
	bt_encode(interval & 0xFF, &bt_data.send_buffer[4]);

	if (duration == 0)
		bt_data.send_cursor = 6;
	else
	{
		bt_data.send_buffer[6] = ',';
		bt_encode(duration >> 8, &bt_data.send_buffer[7]);
		bt_encode(duration & 0xFF, &bt_data.send_buffer[8]);
		bt_data.send_cursor = 9;
	}

	result = bt_cmd_sync(NULL, kBT_CommandPreloaded | kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	result = bt_disable_module();
	return kBT_NoError;
}

BluetoothResult bt_setfeatures(uint32_t services)
{
	BluetoothResult result;
	int i;
	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	//Copy the command over
	bt_data.send_buffer[0] = 'S';
	bt_data.send_buffer[1] = 'R';
	bt_data.send_buffer[2] = ',';
	bt_data.send_cursor = 3;

	for (i=3; i>=0; --i)
	{
		uint8_t *data = (uint8_t*)&services;
		bt_encode(data[i], &bt_data.send_buffer[bt_data.send_cursor]);
		bt_data.send_cursor += 2;
	}

	result = bt_cmd_sync(NULL, kBT_CommandPreloaded | kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	return bt_disable_module();
}

BluetoothResult bt_setservices(uint32_t services)
{
	BluetoothResult result;
	int i;
	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	//Copy the command over
	bt_data.send_buffer[0] = 'S';
	bt_data.send_buffer[1] = 'S';
	bt_data.send_buffer[2] = ',';
	bt_data.send_cursor = 3;

	for (i=3; i>=0; --i)
	{
		uint8_t *data = (uint8_t*)&services;
		bt_encode(data[i], &bt_data.send_buffer[bt_data.send_cursor]);
		bt_data.send_cursor += 2;
	}

	result = bt_cmd_sync(NULL, kBT_CommandPreloaded | kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	return bt_disable_module();
}

BluetoothResult bt_setname(const char *name)
{
	BluetoothResult result;
	unsigned int len = strlen(name);

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	//Copy the command over
	bt_data.send_buffer[0] = 'S';
	bt_data.send_buffer[1] = '-';
	bt_data.send_buffer[2] = ',';
	bt_data.send_cursor = 3;

	strcpy(bt_data.send_buffer+3, name);
	bt_data.send_cursor += len;

	result = bt_cmd_sync(NULL, kBT_CommandPreloaded | kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	return bt_reboot(1);
}

/* 
 * Start broadcasting a message.  Data contains the payload to send.  
 * it must be <= 25 bytes per the RN4020 user manual which are hex encoded
 * for transmission. 
 *
 */

BluetoothResult bt_broadcast(const char *data, unsigned int length)
{
	BluetoothResult result;
	unsigned int i;

	if (length > 25)
		return kBT_SendOverflow;

	result = bt_enable_module();
	if (result != kBT_NoError)
		return result;

	//Copy the command over
	bt_data.send_buffer[0] = 'N';
	bt_data.send_buffer[1] = ',';
	bt_data.send_cursor = 2;

	for (i=0; i<length; ++i)
	{
		bt_encode(data[i], &bt_data.send_buffer[bt_data.send_cursor]);
		bt_data.send_cursor += 2;
	}

	result = bt_cmd_sync(NULL, kBT_CommandPreloaded | kBT_ParseResponse, 0);
	if (result != kBT_NoError)
	{
		bt_disable_module();
		return result;
	}

	result = bt_disable_module();
	return result;
}
