#include "serial.h"
#include <string.h>
#include <stdarg.h>

volatile char rcv_buffer[UART_BUFFER_SIZE];
volatile char send_buffer[UART_BUFFER_SIZE];

extern char command_buffer[UART_BUFFER_SIZE];

volatile char *rcv_cursor;

volatile char *send_cursor;
volatile unsigned char sending;

void configure_uart(uart_parameters *params)
{
    //calculate the appropriate baud setting
    if (params->baud > HIBAUDMIN)
    {
        U1MODEbits.BRGH = 1;
        U1BRG = CALC_BAUDHI(params->baud);
    }
    else
    {
        U1MODEbits.BRGH = 0;
        U1BRG = CALC_BAUDLO(params->baud);
    }

    U1MODEbits.USIDL = 0; //Operate in idle mode
    U1MODEbits.IREN = 0;

    if (params->hw_flowcontrol)
        U1MODEbits.UEN = 0b10; //Use cts and rts
    else
        U1MODEbits.UEN = 0b00; //Don't use cts and rts

    U1MODEbits.ABAUD = 0; //don't infer baud from the line
    U1MODEbits.PDSEL = params->parity & 0b11; //set parity
    U1MODEbits.STSEL = 0; //1 stop bit

    //setup the interrupts
    rcv_cursor = rcv_buffer;
    rcv_buffer[0] = '\0';

    send_cursor = send_buffer;
    send_buffer[0] = '\0';
    sending = 0;

    U1STAbits.UTXISEL1 = 1; //Transmit interrupt when FIFO becomes empty
    U1STAbits.UTXISEL0 = 0;
    U1STAbits.URXISEL  = 0b00; //Receive interrupt when any character is received

    //Clear receive buffer
    while(U1STAbits.URXDA == 1) 
    {
        char Temp;
        Temp = U1RXREG;
    }

    IFS0bits.U1RXIF = 0;  //Clear ISF flags
    IFS0bits.U1TXIF = 0;
    IPC2bits.U1RXIP = 0b010; //Set high priority
    IPC3bits.U1TXIP = 0b100;
    IEC0bits.U1RXIE = 1; //Enable both interrupts
    IEC0bits.U1TXIE = 1;

    U1MODEbits.UARTEN = 1; //Enable the uart
    U1STAbits.UTXEN = 1; //Enable transmission

    _LATA0 = 0;
}

//Interrupt Handlers
void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
    //_LATA0 = !_LATA0; //flip light just to make sure this is working.

    while(U1STAbits.URXDA == 1)
    {
        if (rcv_cursor == rcv_buffer+UART_BUFFER_SIZE)
        {
            rcv_cursor = rcv_buffer;
            sends("Command too long.\n");

        }

        *(rcv_cursor) = U1RXREG;

        //Check if we've received an entire command
        if (*rcv_cursor == '\n')
        {
            *rcv_cursor = '\0';
            strncpy(command_buffer, rcv_buffer, UART_BUFFER_SIZE);

            //FIXME: move this to a task executed in the main loop
            process_command();

            //Reset everything and send a command prompt
            rcv_cursor = rcv_buffer;
            sends("PIC 24f16ka101> ");

            break;
        }   

        rcv_cursor = rcv_cursor+1;
    }

    IFS0bits.U1RXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
    if (sending)
    {
        if (*send_cursor != '\0')
        {
            while (_UTXBF == 0 && *send_cursor != '\0')
                U1TXREG = *send_cursor++;
        }

        if (*send_cursor == '\0')
            sending = 0;
    }
    
    IFS0bits.U1TXIF = 0; //Clear IFS flag
}

void send(char *msg)
{
    sending = 0;
    strncpy(send_buffer, msg, UART_BUFFER_SIZE);
    send_cursor = send_buffer+1;
    sending = 1;

    U1TXREG = send_buffer[0];
}

void sends(char *msg)
{
    send(msg);

    while (sending)
        ;
}

void sendf(char *fmt, ...)
{
    sending = 0;
    va_list argp;
    va_start(argp, fmt);

    vsnprintf(send_buffer, UART_BUFFER_SIZE, fmt, argp);

    va_end(argp);

    send_cursor = send_buffer+1;
    sending = 1;

    U1TXREG = send_buffer[0];
}
