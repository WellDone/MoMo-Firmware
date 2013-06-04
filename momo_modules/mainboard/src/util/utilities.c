#include "utilities.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char get_2byte_number(char *input)
{
    char temp[3];

    temp[0] = input[0];
    temp[1] = input[1];
    temp[2] = '\0';

    return (unsigned char)atoi(temp);
}

/*
 * A simplified version of printf that can only handle printing strings and
 * decimal numbers because apparently vsnprintf is takes 75% of the pic's
 * memory.
 */
void sprintf_small(char *buffer, unsigned int len, const char *fmt, va_list argp)
{
    unsigned int i=0;

    while(*fmt != '\0')
    {
        if (i>=len)
        {
            buffer[len-1] = '\0';
            return;
        }

        if (*fmt != '%')
        {
            *buffer++ = *fmt++;
            ++i;
            continue;
        }

        else
        {
            //Process a format specifier
            //The next character must be either s, d or %
            ++fmt;

            switch(*fmt)
            {
                case 'd':
                {
                    int num = va_arg(argp, int);
                    int skip_len = itoa_small(buffer,len-i, num);
                    buffer += skip_len;
                    break;
                }

                case 's':
                {
                    char *str = va_arg(argp, char *);
                    strncpy(buffer, str, len-i);
                    i += strlen(str);
                    buffer += strlen(str);
                    break;
                }
                case '%':
                {
                    *buffer++ = '%';
                    ++i;
                    break;
                }
                case '\0':
                {
                    goto end;
                }
            }
            ++fmt; //get past the format specifier
        }
    }

end:
    *buffer = '\0';
}

/*
 * Quick and dirty version of itoa for 16 bit numbers,
 * with support for negative numbers.
 *
 */
int itoa_small(char *buf, unsigned int len, int num)
{
    char temp[10];
    unsigned int i=9;
    int negative = 0;

    if (num < 0)
    {
        negative = 1;
        num *= -1; //FIXME, there are faster ways of inverting a number
    }

    temp[i--] = '\0';

    if (num == 0)
    {
        temp[i--] = '0';
    }
    else
    {
        while (num > 0)
        {
            char rem = (char)(num % 10);
            num /= 10;

            temp[i--] = '0' + rem;
        }

        if (negative)
            temp[i--] = '-';
    }


    memcpy(buf, temp+i+1, 9-i-1);

    return 9-i-1;
}

bool atoi_small(const char* buf, int* out)
{
    *out = 0x0;
    int i;
    unsigned int *uout = (unsigned int*) out;
    for ( i=0; i<strlen(buf); ++i ) {
        if ( buf[i] == '-' && i==0 ) {
            *uout &= 0x80000000; // -0
        } else if ( buf[i] >= '0' && buf[i] <= '9' ) {
            *uout = *uout * 10;
            *uout += buf[i]-'0';
        } else {
            return false;
        }
    }
    return true;
}

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