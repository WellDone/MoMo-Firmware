#ifndef __bus_slave_h__
#define __bus_slave_h__

#include "bus.h"

//Slave Routines
void bus_slave_read_callback();
void bus_slave_startcommand();

//Defined in bus_slave_asm.as
void bus_slave_setreturn(uint8 status);
void bus_slave_checkparamsize();
uint8 bus_retval_size();

#endif
