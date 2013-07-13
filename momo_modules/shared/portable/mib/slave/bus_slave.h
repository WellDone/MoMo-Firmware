#ifndef __bus_slave_h__
#define __bus_slave_h__

#include "bus.h"

//Slave Routines
void bus_slave_callback();
void bus_slave_reset();
void bus_slave_seterror(unsigned char error);
void bus_slave_setreturn(unsigned char status, MIBParameterHeader *value);

#endif
