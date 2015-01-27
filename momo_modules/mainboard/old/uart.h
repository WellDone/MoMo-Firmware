#ifndef __uart_h__
#define __uart_h__

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
    CallEachCharacter = 0,
    CallEachNewline
} ReceiveSettings;

typedef struct
{
    unsigned long baud;
    unsigned char hw_flowcontrol; //nonzero to use rts/cts

    ParitySettings parity;
} uart_parameters;

typedef struct
{
    volatile char rcv_buffer[UART_BUFFER_SIZE];
    volatile char send_buffer[UART_BUFFER_SIZE];

    volatile char volatile *rcv_cursor;
    volatile unsigned char receiving;
    volatile char volatile *send_cursor;
    volatile unsigned char sending;
} UART_STATUS;

void configure_uart( UARTPort port, uart_parameters *params);
void dump_gsm_buffer(void);
void put( UARTPort port, const char c );
void send( UARTPort port, const char *msg );
void sends( UARTPort port, const char *msg ); //Blocking send function
void sendf( UARTPort port, const char *fmt, ... );

int read( UARTPort port, char** buffer, const char* stopString );
int readln( UARTPort port, char** buffer );

#endif
