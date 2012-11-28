#include "serial.h"
#include <string.h>
#include <stdarg.h>
#include <PIC24F_plib.h>

#define CALC_BAUDHI(baud)     (unsigned int)((CLOCKSPEED/(4*baud))-1)    //Assumes hi speed
#define CALC_BAUDLO(baud)     (unsigned int)((CLOCKSPEED/(16*baud))-1)    //Assumes low speed
#define HIBAUDMIN             CLOCKSPEED/(16L*65536L)
#define CALC_BAUD(baud)       ( baud > HIBAUDMIN )?CALC_BAUDHI:CALC_BAUDLO

//Define ISR Vectors for UART1
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

typedef struct
{
    volatile char rcv_buffer[UART_BUFFER_SIZE];
    volatile char send_buffer[UART_BUFFER_SIZE];

    volatile char *rcv_cursor;
    volatile unsigned char receiving;
    volatile char *send_cursor;
    volatile unsigned char sending;
} UART_STATUS;

static UART_STATUS uart_stats[2];

void configure_uart1(uart_parameters *params)
{
    OpenUART1();
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

void configure_uart2(uart_parameters *params)
{
    //calculate the appropriate baud setting
    if (params->baud > HIBAUDMIN)
    {
        U2MODEbits.BRGH = 1;
        U2BRG = CALC_BAUDHI(params->baud);
    }
    else
    {
        U2MODEbits.BRGH = 0;
        U2BRG = CALC_BAUDLO(params->baud);
    }

    U2MODEbits.USIDL = 0; //Operate in idle mode
    U2MODEbits.IREN = 0;

    if (params->hw_flowcontrol)
        U2MODEbits.UEN = 0b10; //Use cts and rts
    else
        U2MODEbits.UEN = 0b00; //Don't use cts and rts

    U2MODEbits.ABAUD = 0; //don't infer baud from the line
    U2MODEbits.PDSEL = params->parity & 0b11; //set parity
    U2MODEbits.STSEL = 0; //1 stop bit

    //setup the interrupts
    u2stat.rcv_cursor = u2stat.rcv_buffer;
    u2stat.rcv_buffer[0] = '\0';

    u2stat.send_cursor = u2stat.send_buffer;
    u2stat.send_buffer[0] = '\0';
    u2stat.sending = 0;

    U2STAbits.UTXISEL1 = 1; //Transmit interrupt when FIFO becomes empty
    U2STAbits.UTXISEL0 = 0;
    U2STAbits.URXISEL  = 0b00; //Receive interrupt when any character is received

    //Clear receive buffer
    while(U2STAbits.URXDA == 1)
    {
        char Temp;
        Temp = U2RXREG;
    }

    IFS1bits.U2RXIF = 0;  //Clear ISF flags
    IFS1bits.U2TXIF = 0;
    IPC7bits.U2RXIP = 0b010; //Set high priority
    IPC7bits.U2TXIP = 0b100;
    IEC1bits.U2RXIE = 1; //Enable both interrupts
    IEC1bits.U2TXIE = 1;

    U2MODEbits.UARTEN = 1; //Enable the uart
    U2STAbits.UTXEN = 1; //Enable transmission

    _LATA0 = 0;
}

void configure_uart( UARTPort port, uart_parameters *params)
{
    switch ( port )
    {
        case UART1:
            OpenUART1( UART_EN,
                       UART_TX_ENABLE,
                       CALC_BAUD( params->baud ) );
            ConfigIntUART1( UART_RX_INT_PR2 | UART_TX_INT_PR2 )
            break;
        case UART2:
            
            break;
    }
}

static UART_STATUS& GetStatus( UARTPort port )
{
    return uart_stats[port];
}

//Interrupt Handlers
void _RXInterrupt( UART_STATUS* stat)
{
    //_LATA0 = !_LATA0; //flip light just to make sure this is working.

    while(U1STAbits.URXDA == 1)
    {
        if (stat.rcv_cursor == stat.rcv_buffer+UART_BUFFER_SIZE)
        {
            stat.rcv_cursor = stat.rcv_buffer;
            sends("Command too long.\n");

        }

        *(stat.rcv_cursor) = U1RXREG;

        //Check if we've received an entire command
        if (*stat.rcv_cursor == '\n')
        {
            *stat.rcv_cursor = '\0';
            strncpy(command_buffer, stat.rcv_buffer, UART_BUFFER_SIZE);

            //FIXME: move this to a task executed in the main loop
            process_command();

            //Reset everything and send a command prompt
            stat.rcv_cursor = stat.rcv_buffer;
            sends("PIC 24f16ka101> ");

            break;
        }   

        stat.rcv_cursor = stat.rcv_cursor+1;
    }
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
    if (u1stat.sending)
    {
        if (*u1stat.send_cursor != '\0')
        {
            while (_UTXBF == 0 && *u1stat.send_cursor != '\0')
                U1TXREG = *u1stat.send_cursor++;
        }

        if (*u1stat.send_cursor == '\0')
            u1stat.sending = 0;
    }
    
    IFS0bits.U1TXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
    _RXInterrupt( GetStatus( UART1 ) );
    IFS0bits.U1RXIF = 0; //Clear IFS flag
}

void put( UARTPort port, char c )
{
    UART_STATUS& stat = GetStatus( port );
    stat.sending = 0;
    while ( stat.send_cursor == stat.send_buffer + UART_BUFFER_SIZE )
        ; //TODO: Do this better.
    *stat.send_cursor = c;
}

void send(UARTPort port, char *msg)
{
    UART_STATUS& stat = GetStatus( port );
    stat.sending = 0;
    strncpy(stat.send_buffer, msg, UART_BUFFER_SIZE);
    stat.send_cursor = stat.send_buffer+1;
    stat.sending = 1;

    U1TXREG = stat.send_buffer[0];
}

void sends(UARTPort port, char *msg)
{
    send(msg);

    UART_STATUS& stat = GetStatus( port );
    while (stat.sending)
        ;

    //Even after we stop filling the transmit FIFO, wait until the last bit is
    //shifted out.  Fixes a bug where the device reset command will not be
    //able to send the last 4 bits of its message because the device resets
    //with those characters in the transmit shift register.
    while (_TRMT)
        ;
}

void sendf(UARTPort port, char *fmt, ...)
{
    UART_STATUS& stat = GetStatus( port );
    stat.sending = 0;
    va_list argp;
    va_start(argp, fmt);

    sprintf_small(stat.send_buffer, UART_BUFFER_SIZE, fmt, argp);

    va_end(argp);

    stat.send_cursor = stat.send_buffer+1;
    stat.sending = 1;

    U1TXREG = stat.send_buffer[0];

    while (stat.sending)
        ;
}
