#ifndef __common_types_h__
#define __common_types_h__

#include <GenericTypeDefs.h>

typedef unsigned short bool;
#define false 0
#define true 1

// UART
typedef enum
{
    U1 = 0,
    U2 = 1
} UARTPort;

#endif