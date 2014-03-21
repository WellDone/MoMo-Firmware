#ifndef __bus_slave_h__
#define __bus_slave_h__

#include "bus.h"

//Slave Routines
void bus_slave_read_callback();
void bus_slave_startcommand();

//Defined in bus_slave_asm.as
void bus_slave_setreturn(uint8 status);
uint8 bus_slave_checkparamsize();
uint8 bus_retval_size();

#define mib_success() bus_slave_setreturn( pack_return_status(0,0) )
#define mib_return(return_length) bus_slave_setreturn( pack_return_status( 0, return_length ) )
#define mib_error(code,return_length) bus_slave_setreturn( pack_return_status( code, return_length ) )

#endif
