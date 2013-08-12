/* gsmstream.c
 *
 * Endpoints for opening and dumping data to a text message stream.
 *
 *
 *
 */

#include "mib12_api.h"

 bit stream_open: 1;
 uint8 bytes_remaining;

 void gsm_openstream()
 {
 	if (GSMSTATUSPIN == 0)
 	{
 		bus_slave_setreturn(pack_return_status(6,0));
 		return;
 	}

 	


 }