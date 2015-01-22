#ifndef __utilities_h__
#define __utilities_h__

#include <stdarg.h>
#include <stdint.h>
#include "common_types.h"
#include "bit_utilities.h"

unsigned char get_2byte_number(const char *input);
unsigned int sprintf_small(char *buffer, unsigned int len, const char *fmt, va_list argp);
int itoa_small(char *buf, unsigned int len, int num);
bool atoi_small(const char *buf, int* out);

char to_upper_case( char c );

void print_byte( uint8_t b );
void println( const char* msg );

uint8_t hexbyte_to_binary( char* ascii );
void binary_to_hexbyte( uint8_t b, char* out );

#endif
