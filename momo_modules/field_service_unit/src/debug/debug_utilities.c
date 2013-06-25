#include "debug_utilities.h"
#include "common.h"

void print_byte( BYTE b )
{
  print( "0b");
  BYTE mask = 0x80;
  while (mask!=0x0) {
    if ( ( b & mask ) )
      print("1");
    else
      print("0");
    mask = mask >> 1;
  }
  print("\r\n");
}

void println( const char* msg ) {
  print( msg );
  print( "\r\n" );
}