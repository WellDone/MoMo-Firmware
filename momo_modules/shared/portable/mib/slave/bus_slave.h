#ifndef __bus_slave_h__
#define __bus_slave_h__

#include "bus.h"

//Slave Routines
void bus_slave_callback();
void bus_slave_reset();
void bus_slave_setreturn(uint8 status);

#endif
