#ifndef __uart_h__
#define __uart_h__

#include "interrupts.h"
#include "pic24.h"
#include "ringbuffer.h"

#define UART_BUFFER_SIZE        64
#define kUARTHeartbeatCharacter 255

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

typedef enum
{
    U1 = 0,
    U2 = 1
} UARTPort;

typedef enum
{
    kDiscardReceivedCharactersFlag = 0,
    kResetOnNullCharacterFlag = 1,
    kEnableHeartbeatFlag = 2
} UARTFlag;

typedef void(*uart_newline_callback)(char* buffer, int length, bool overflown);
typedef void(*uart_char_callback)(char data);

typedef struct
{
    volatile char           send_buffer_data[UART_BUFFER_SIZE];

    ringbuffer              send_buffer;

    uart_char_callback      rx_callback;
    uart_newline_callback   rx_newline_callback;

    volatile char*          rx_linebuffer;
    volatile char*          rx_linebuffer_cursor;
    volatile int            rx_linebuffer_remaining;
    unsigned int            rx_linebuffer_length;

    unsigned int            flags;
} UART_STATUS;

void configure_uart( UARTPort port, uart_parameters *params);

void uart_set_newline_callback( UARTPort port, uart_newline_callback callback, volatile char *linebuffer, unsigned int buffer_length);
void clear_uart_rx_newline_callback( UARTPort port );

void uart_clear_receive_buffer(UARTPort port);
void uart_set_flag(UARTPort port, UARTFlag flag, unsigned char value);
unsigned int uart_get_flag(UARTPort port, UARTFlag flag);


void put( UARTPort port, const char c );
void send( UARTPort port, const char *msg );
void sends( UARTPort port, const char *msg ); //Blocking send function
void sendf( UARTPort port, const char *fmt, ... );

unsigned int readln_sync( UARTPort port, char* buffer, int buffer_length );

#endif
