#ifndef __intel_hex_h
#define __intel_hex_h

#include "common_types.h"

#define HEX_DATA_REC        0   /* data record */
#define HEX_EOF_REC         1   /* end of file record */
#define HEX_EXTADDR_REC     4   /* extened linear address record */

typedef struct {
	uint16 address;
	uint8  record_type;
	uint8  data[16];
} intel_hex16_body;

typedef struct {
	uint8            data_length;
	intel_hex16_body body;	
	uint8            checksum;
} intel_hex16;

#define intel_hex_startcode ':'
typedef struct {
	char startcode;
	char data_length [2];
	char address     [4];
	char record_type [2];
	char data        [16];
	char checksum    [2];
} intel_hex16_ascii;

bool compress_intel_hex16_ascii( intel_hex16_ascii* in, intel_hex16* out, uint8 in_len );
void expand_intel_hex16_binary( intel_hex16* in, intel_hex16_ascii* out );

#endif