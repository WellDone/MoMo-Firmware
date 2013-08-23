#ifndef __utilities_h__
#define __utilities_h__

#include <stdarg.h>
#include "common_types.h"
#include "bit_utilities.h"

unsigned char get_2byte_number(const char *input);
unsigned int sprintf_small(char *buffer, unsigned int len, const char *fmt, va_list argp);
int itoa_small(char *buf, unsigned int len, int num);
bool atoi_small(const char *buf, int* out);

void print_byte( BYTE b );
void println( const char* msg );

BYTE hexbyte_to_binary( char* ascii );
void binary_to_hexbyte( BYTE b, char* out );

#endif
