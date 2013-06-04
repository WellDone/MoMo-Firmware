//sendcommand.h

#include "sendcommand.h"

/* 
 * Implement state machine to send a command 
 */

unsigned char volatile response;

int sendcommand_master_callback(int state)
{
	if (state == kCommandBegin)
		bus_receive(bus_master_lastaddress(), &response, 1, 0);
	
	return kCommandFinished;
}