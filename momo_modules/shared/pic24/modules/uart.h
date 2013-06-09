#ifndef __uart_h__
#define __uart_h__

#include "interrupts.h"
#include "common_types.h"
#include "ringbuffer.h"

#define UART_BUFFER_SIZE      99

typedef enum
{
    OddParity = 0b10,
    EvenParity = 0b01,
    NoParity = 0b00
} ParitySettings;

typedef struct
{
    unsigned long baud;
    unsigned char hw_flowcontrol; //nonzero to use rts/cts

    ParitySettings parity;
} uart_parameters;

typedef struct
{
    volatile char rcv_buffer_data[UART_BUFFER_SIZE];
    volatile char send_buffer_data[UART_BUFFER_SIZE];

    ringbuffer rcv_buffer;
    ringbuffer send_buffer;

    void(*rx_callback)(char data);
    void(*rx_newline_callback)(int length, bool overflown);

    volatile char* rx_linebuffer;
    volatile char* rx_linebuffer_cursor;
    volatile int   rx_linebuffer_remaining;
} UART_STATUS;

void configure_uart( UARTPort port, uart_parameters *params);

void set_uart_rx_char_callback( UARTPort port, void (*callback)(char) );
void clear_uart_rx_char_callback( UARTPort port );
void set_uart_rx_newline_callback( UARTPort port, void (*callback)(int length, bool overflown), char *linebuffer, int buffer_length );
void clear_uart_rx_newline_callback( UARTPort port );


void put( UARTPort port, const char c );
void send( UARTPort port, const char *msg );
void sends( UARTPort port, const char *msg ); //Blocking send function
void sendf( UARTPort port, const char *fmt, ... );

unsigned int readln( UARTPort port, char* buffer, int buffer_length );

#endif
