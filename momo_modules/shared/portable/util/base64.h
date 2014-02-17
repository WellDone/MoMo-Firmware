#ifndef __base64_h__
#define __base64_h__

#include "platform.h"

#ifndef _PIC12LEAN

#include "common_types.h"

int base64_encode( const BYTE* data, unsigned int input_length, char* encoded_data, unsigned int out_size );
int base64_decode(const char* encoded_data, unsigned int input_length, BYTE* data, unsigned int out_size );
unsigned char decode_one(char c);
#endif

#endif