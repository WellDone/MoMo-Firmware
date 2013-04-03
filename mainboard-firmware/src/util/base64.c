#include "base64.h"

static char encoding_table[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[3] = {0, 2, 1};

unsigned int base64_encode( const BYTE* data, unsigned int input_length, char* encoded_data, unsigned int out_size )
{
  unsigned int output_length;
  int i,j;
  BYTE octet_a, octet_b, octet_c;
  unsigned long triple;
  output_length = 4 * ((input_length + 2) / 3);
  if ( output_length > out_size )
    return 0;

  for (i = 0, j = 0; i < input_length;) {
      octet_a = data[i++];
      octet_b = i < input_length ? data[i++] : 0;
      octet_c = i < input_length ? data[i++] : 0;

      triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

      encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
      encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (i = 0; i < mod_table[input_length % 3]; ++i)
      encoded_data[output_length - 1 - i] = '=';

  return output_length;
}