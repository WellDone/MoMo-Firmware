#include "base64.h"

#ifndef _PIC12LEAN

static const char encoding_table[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static int mod_table[3] = {0, 2, 1};

int base64_encode( const BYTE* data, unsigned int input_length, char* encoded_data, unsigned int out_size )
{
  unsigned int output_length;
  int i,j;
  unsigned long octet_a, octet_b, octet_c;
  unsigned long triple;
  output_length = 4 * ((input_length + 2) / 3);
  if ( output_length > out_size )
    return 0;

  for (i = 0, j = 0; i < input_length;) {
      octet_a = (i < input_length) ? data[i++] : 0;
      octet_b = (i < input_length) ? data[i++] : 0;
      octet_c = (i < input_length) ? data[i++] : 0;

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

int base64_decode(const char* encoded_data, unsigned int input_length, BYTE* data, unsigned int out_size )
{
  unsigned int out_length;
  unsigned int i, j;

  if (input_length % 4 != 0)
    return -1;

  out_length = (input_length>>2)*3;

  //Check if there are padding characters
  if (encoded_data[input_length-1] == '=')
    out_length -= 1;
  if (encoded_data[input_length-2] == '=')
    out_length -= 1;
  
  if (out_length > out_size)
    return -1;

  for (i=0, j=0; i<input_length; i+=4)
  {
    unsigned long c1, c2, c3, c4;
    unsigned long quad;

    c1 = decode_one(encoded_data[i+0]);
    c2 = decode_one(encoded_data[i+1]);
    c3 = decode_one(encoded_data[i+2]);
    c4 = decode_one(encoded_data[i+3]);

    //each c1-c4 is 6 bits long with high bits set to zero, so or combination is okay
    quad = c1 << 18 | c2 << 12 | c3 << 6 | c4;

    data[j++] = (quad & 0x00FF0000) >> 16;

    //Check to make sure we have a full octet
    if (encoded_data[i+2] == '=' && encoded_data[i+2] == '=')
      break;

    data[j++] = (quad & 0x0000FF00) >> 8;

    if (encoded_data[i+2] == '=')
      break;

    data[j++] = (quad & 0x000000FF);
  }

  return out_length;
}

/*
 * decode one base64 character without using a lookup table
 */

unsigned char decode_one(char c)
{
  if (c >= 'A' && c <= 'Z')
    return c - 'A';

  if (c >= 'a' && c <= 'z')
    return c - 'a' + 26;

  if (c >= '0' && c <= '9')
    return c - '0' + 52;

  if ( c == '+')
    return 62;

  if (c == '/')
    return 63;

  return 0;
}

#endif