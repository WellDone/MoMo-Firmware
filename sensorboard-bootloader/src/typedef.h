#ifndef _ADDRESS_TYPEDEF
#define _ADDRESS_TYPEDEF

typedef union _ADDRESS
{
    struct
    {
        int address;
    } word;

    struct
    {
        unsigned char byte_L;
        unsigned char byte_H;
    } bytes;
} ADDRESS;

#endif