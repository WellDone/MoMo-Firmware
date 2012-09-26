//serial.h

#ifndef __serial_h__
#define __serial_h__

#include "interrupts.h"
#include "common.h"

#define CALC_BAUDHI(baud)     (unsigned int)((CLOCKSPEED/(4*baud))-1)    //Assumes hi speed
#define CALC_BAUDLO(baud)     (unsigned int)((CLOCKSPEED/(16*baud))-1)    //Assumes low speed
#define HIBAUDMIN             CLOCKSPEED/(16L*65536L)
#define UART_BUFFER_SIZE      99

//Define ISR Vectors for UART
#define U1RX_ISR_DESCRIPTOR (isr_descriptor){ \
                                0b100,        \
                                0,            \
                                (unsigned int*)IEC0,         \
                                (unsigned int*)IPC2,        \
                                (unsigned int*)IFS0,         \
                                11,           \
                                12,            \
                                11}

#define U1TX_ISR_DESCRIPTOR (isr_descriptor){ \
                                0b100,        \
                                0,            \
                                (unsigned int*)IEC0,         \
                                (unsigned int*)IPC3,        \
                                (unsigned int*)IFS0,         \
                                12,           \
                                0,            \
                                12}

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

void configure_uart(uart_parameters *params);
void send(char *msg);
void sends(char *msg); //Blocking send function
void sendf(char *fmt, ...);


//interrupts
void handle_uart1rx();
void handle_uart1tx();
#endif
