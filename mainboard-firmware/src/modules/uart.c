#include "uart.h"
#include "utilities.h"
#include "debug/serial_commands.h"
#include <string.h>
#include <stdarg.h>

#define CALC_BAUDHI(baud)     (unsigned int)((CLOCKSPEED/(4*baud))-1)    //Assumes hi speed
#define CALC_BAUDLO(baud)     (unsigned int)((CLOCKSPEED/(16*baud))-1)    //Assumes low speed
#define HIBAUDMIN             CLOCKSPEED/(16L*65536L)
#define CALC_BAUD(baud)       ( baud > HIBAUDMIN )?CALC_BAUDHI:CALC_BAUDLO

UART_STATUS __attribute__((space(data))) uart_stats[2];

#define u1stat uart_stats[0]
#define u2stat uart_stats[1]

extern volatile char* command_buffer;
extern volatile int cmd_ready;
extern volatile int cmd_received;

void dump_gsm_buffer(void)
{
    int diff = u1stat.rcv_cursor - u1stat.rcv_buffer;
    int i;

    sendf(U2, "num chars: %d\r\n", diff);

    *u1stat.rcv_cursor ='\0';

/*    for (i=0;i<UART_BUFFER_SIZE; ++i)
    {
        if (u1stat.rcv_buffer[i] == '\r')
            u1stat.rcv_buffer[i] = 'r';
        else if (u1stat.rcv_buffer[i] == '\n')
            u1stat.rcv_buffer[i] = 'n';
    }*/

    print(u1stat.rcv_buffer);
    u1stat.rcv_cursor = u1stat.rcv_buffer;
}

void uart_set_disabled(UARTPort port, int status)
{
    if (port == U1)
    {
        _U1MD = status;
    }
    else
    {
        _U2MD = status;
    }
}

void configure_uart1(uart_parameters *params)
{
    //calculate the appropriate baud setting
    if (params->baud > HIBAUDMIN)
    {
        U1MODEbits.BRGH = 1;
        U1BRG = 16;//69;//CALC_BAUDHI(params->baud); FIXME
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
    U1MODEbits.STSEL = 0; //1 stop bits; CHECK

    //setup the interrupts
    u1stat.rcv_cursor = u1stat.rcv_buffer;
    u1stat.rcv_buffer[0] = '\0';

    u1stat.send_cursor = u1stat.send_buffer;
    u1stat.send_buffer[0] = '\0';
    u1stat.sending = 0;

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
    u2stat.receiving = 0;

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
    IEC1bits.U2RXIE = 1; //Enable receive interrupt
    IEC1bits.U2TXIE = 1; //Enable transmit interrupt

    U2MODEbits.UARTEN = 1; //Enable the uart
    U2STAbits.UTXEN = 1; //Enable transmission
}

void configure_uart( UARTPort port, uart_parameters *params)
{
    switch ( port )
    {
        case U1:
            configure_uart1( params );
            break;
        case U2:
            configure_uart2( params );
            break;
    }
}

static UART_STATUS* GetStatus( UARTPort port )
{
    return &uart_stats[port];
}

//Interrupt Handlers
void receive_command( UART_STATUS* stat)
{
    while(U2STAbits.URXDA == 1)
    {
        if (stat->rcv_cursor == stat->rcv_buffer+UART_BUFFER_SIZE)
        {
            stat->rcv_cursor = stat->rcv_buffer;
            print( "Command too long.\r\n");

        }

        *(stat->rcv_cursor) = U2RXREG;

        //Check if we've received an entire command
        if (*stat->rcv_cursor == '\n')
        {
            //Check if they sent \r\n and chomp it.
            if (stat->rcv_cursor != stat->rcv_buffer && *(stat->rcv_cursor-1) == '\r')
                --stat->rcv_cursor;

            *stat->rcv_cursor = '\0';

            //Reset everything
            stat->rcv_cursor = stat->rcv_buffer;

            //Signal the main loop task to process the command
            cmd_ready = 1;
            cmd_received = 1; //Set the global command received flag to keep the debug interface alive.
            taskloop_add(process_commands_task);
            break;
        }

        stat->rcv_cursor = stat->rcv_cursor+1;
    }
}

void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
   UART_STATUS *stat = &u1stat;
   while(U1STAbits.URXDA == 1)
    {
        if (stat->rcv_cursor == stat->rcv_buffer+UART_BUFFER_SIZE)
        {
            stat->rcv_cursor = stat->rcv_buffer;
            //print( "GSM buffer full.\r\n");

        }

        *(stat->rcv_cursor) = U1RXREG;
        //U2TXREG = *(stat->rcv_cursor); //echo first four characters
        //put( U2, *(stat->rcv_cursor) ); //echo to debug output

        stat->rcv_cursor = stat->rcv_cursor+1;
    }

    IFS0bits.U1RXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,auto_psv)) _U2RXInterrupt()
{
    receive_command( &u2stat );

    IFS1bits.U2RXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
    if (u1stat.sending)
    {
        if (*u1stat.send_cursor != '\0')
        {
            while (U1STAbits.UTXBF == 0 && *u1stat.send_cursor != '\0')
                U1TXREG = *u1stat.send_cursor++;
        }

        if (*u1stat.send_cursor == '\0')
            u1stat.sending = 0;
    }

    IFS0bits.U1TXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U2TXInterrupt()
{
    if (u2stat.sending)
    {
        if (*u2stat.send_cursor != '\0')
        {
            while (U2STAbits.UTXBF == 0 && *u2stat.send_cursor != '\0')
                U2TXREG = *u2stat.send_cursor++;
        }

        if (*u2stat.send_cursor == '\0')
            u2stat.sending = 0;
    }
    IFS1bits.U2TXIF = 0; //Clear IFS flag
}

void put( UARTPort port, const char c )
{
    char* buf[2];
    buf[0] = c;
    buf[1] = "\0";
    sends( port, buf );
}

void send(UARTPort port, const char *msg)
{
    //Don't send zero length strings (our logic would be wrong since send_cursor would point past the end of the string in that case.
    if (*msg == '\0')
        return;

    UART_STATUS* stat = GetStatus( port );
    stat->sending = 0;
    strncpy(stat->send_buffer, msg, UART_BUFFER_SIZE);
    stat->send_cursor = stat->send_buffer+1;
    stat->sending = 1;

    if (port == U1)
        U1TXREG = stat->send_buffer[0];
    else
        U2TXREG = stat->send_buffer[0];
}

void sends(UARTPort port, const char *msg)
{
    send(port, msg);

    UART_STATUS* stat = GetStatus( port );
    while (stat->sending)
        ;

    //Even after we stop filling the transmit FIFO, wait until the last bit is
    //shifted out.  Fixes a bug where the device reset command will not be
    //able to send the last 4 bytes of its message because the device resets
    //with those characters in the transmit shift register.
    if (port == U2)
    {
        while (U2STAbits.TRMT == 0) //TRMT = 0 when buffer and shift register empty
            ;
    }
    else
    {
        while (U1STAbits.TRMT == 0)
            ;
    }
}

void sendf(UARTPort port, const char *fmt, ...)
{
    UART_STATUS* stat = GetStatus( port );
    stat->sending = 0;
    va_list argp;
    va_start(argp, fmt);

    sprintf_small(stat->send_buffer, UART_BUFFER_SIZE, fmt, argp);

    va_end(argp);

    stat->send_cursor = stat->send_buffer+1;
    stat->sending = 1;

    if (port == U1)
        U1TXREG = stat->send_buffer[0];
    else
        U2TXREG = stat->send_buffer[0];

    while (stat->sending)
        ;

    //Even after we stop filling the transmit FIFO, wait until the last bit is
    //shifted out.  Fixes a bug where the device reset command will not be
    //able to send the last 4 bytes of its message because the device resets
    //with those characters in the transmit shift register.
    if (port == U2)
    {
        while (U2STAbits.TRMT == 0)
            ;
    }
    else
    {
        while (U1STAbits.TRMT == 0)
            ;
    }
}
