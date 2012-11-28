//serial.h

#ifndef __serial_h__
#define __serial_h__

#include "interrupts.h"
#include "common.h"

#define UART_BUFFER_SIZE      99

typedef enum
{
    OddParity = 0b10,
    EvenParity = 0b01,
    NoParity = 0b00
} ParitySettings;

typedef enum
{
    UART1 = 0,
    UART2 = 1
} UARTPort;

typedef enum
{
    CallEachCharacter = 0,
    CallEachNewline
} ReceiveSettings;

typedef struct
{
    unsigned long baud;
    unsigned char hw_flowcontrol; //nonzero to use rts/cts

    ParitySettings parity;
} uart_parameters;

void configure_uart( UARTPort port, uart_parameters *params);
void putc( UARTPort port, char c );
void send( UARTPort port, char *msg );
void sends( UARTPort port, char *msg ); //Blocking send function
void sendf( UARTPort port, char *fmt, ... );

int read( UARTPort port, char** buffer, const char* stopString );
int readln( UARTPort port, char** buffer );

#endif
