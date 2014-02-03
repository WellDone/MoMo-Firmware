#ifndef __bus_slave_h__
#define __bus_slave_h__

#include "bus.h"

//Slave Routines
void bus_slave_callback(unsigned char was_read);
void bus_slave_return_int16( int16 val );
void bus_slave_return_buffer( const void* buff, uint8 length );
inline void bus_slave_set_returnbuffer_length( uint8 length );

//Defined in bus_slave_asm.as
void bus_slave_setreturn(uint8 status);
uint8 bus_slave_checkparamsize();

uint8 bus_retval_size();

#endif
