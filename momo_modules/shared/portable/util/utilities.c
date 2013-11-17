#include "utilities.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char get_2byte_number(const char *input)
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
unsigned int sprintf_small(char *buffer, unsigned int len, const char *fmt, va_list argp)
{
    unsigned int i=0;

    while(*fmt != '\0')
    {
        if (i>=len)
        {
            buffer[len-1] = '\0';
            return len;
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
                    i += skip_len;
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
    return i;
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

bool atoi_small(const char* buf, int16* out)
{
    *out = 0x0;
    bool negative = false;
    int16 tmp = 0;

    if ( *buf == '-' ) {
        negative = true;
        ++buf;
    }
    while ( *buf != '\0' ) {
        if ( *buf >= '0' && *buf <= '9' ) {
            if ( tmp >= 0x10000/10 )
                return false;
            tmp *= 10;
            tmp += *(buf++) -'0';
        } else {
            return false;
        }
    }
    *out = (negative)?-tmp:tmp;
    return true;
}

char to_upper_case( char c ) {
    if ( 'a' <= c && c <= 'z' )
        return ( c - 'a' ) + 'A';
    else
        return c;
}

BYTE hexbyte_to_binary( char* hex ) {
    uint8 i;
    uint8 ret = 0;
    uint8 ch;
    
    for (i=0; i < 2; i++)
    {
        /* take care of HEX files where lower case letters are used */
        ch = to_upper_case(*hex);
        
        /* convert character to integer value */
        if (ch >= 'A')
        {
            ch = ch - 'A' + 10;
        }
        else
        {
            ch = ch - '0';
        }
        ret = (ret << 4) | ch;
        hex++;
    }
    
    return (ret);
}

static char quad_to_hex( uint8 b )
{
    if (b >= 10)
    {
        b += 'A' - 10;
    }
    else
    {
        b += '0';
    }
    return b;
}
void binary_to_hexbyte( BYTE b, char* out ) {
    out[0] = quad_to_hex( b&0xFF );
    out[1] = quad_to_hex( (b>>4)&0xFF );
}
