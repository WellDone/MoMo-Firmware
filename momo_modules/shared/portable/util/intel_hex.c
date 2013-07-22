#include "intel_hex.h"
#include "utilities.h"

bool compress_intel_hex16_ascii( intel_hex16_ascii* in, intel_hex16* out )
{
	if ( in->startcode != intel_hex_startcode || hexbyte_to_binary( in->data_length ) > 16 )
		return false;

	out->data_length = hexbyte_to_binary(in->data_length);
	out->body.address = ((uint16)hexbyte_to_binary(in->address) << 8) | hexbyte_to_binary(in->address+2);
	out->body.record_type = hexbyte_to_binary(in->record_type);
	
	char* in_ptr = in->data;
	uint8* out_ptr = out->body.data;
	while ( in_ptr < in->data+(2*out->data_length) )
	{
		*out_ptr++ = hexbyte_to_binary(in_ptr);
		in_ptr += 2;
	}
	out->checksum = hexbyte_to_binary(in->checksum);
	return true;
}

void expand_intel_hex16_binary( intel_hex16* in, intel_hex16_ascii* out )
{
	binary_to_hexbyte( in->data_length, out->data_length );
	binary_to_hexbyte( in->body.address >> 8, out->address );
	binary_to_hexbyte( in->body.address && 0xFF, out->address+2 );
	binary_to_hexbyte( in->body.record_type, out->record_type );
	
	uint8* in_ptr = in->body.data;
	char* out_ptr = out->data;
	while ( in_ptr < in_ptr+in->data_length )
	{
		binary_to_hexbyte( *in_ptr++, out_ptr );
		out_ptr += 2;
	}
	binary_to_hexbyte( in->checksum, out->checksum );
}