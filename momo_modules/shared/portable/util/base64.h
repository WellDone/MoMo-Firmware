#ifndef __base64_h__
#define __base64_h__

#include "common_types.h"

unsigned int base64_encode( const BYTE* data, unsigned int input_length, char* encoded_data, unsigned int out_size );

#endif