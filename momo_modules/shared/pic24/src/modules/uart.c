#include "uart.h"
#include "utilities.h"
#include "task_manager.h"
#include "pic24asm.h"
#include <string.h>
#include <stdarg.h>

#define CALC_BAUDHI(baud)     (unsigned int)((CLOCKSPEED/(4*baud))-1)    //Assumes hi speed
#define CALC_BAUDLO(baud)     (unsigned int)((CLOCKSPEED/(16*baud))-1)    //Assumes low speed
#define HIBAUDMIN             CLOCKSPEED/(16L*65536L)
#define CALC_BAUD(baud)       ( baud > HIBAUDMIN )?CALC_BAUDHI:CALC_BAUDLO

UART_STATUS __attribute__((space(data))) uart_stats[2];

#define U1STAT (uart_stats[0])
#define U2STAT (uart_stats[1])

static inline void wait_for_transmission( UARTPort port );

#define STAT(port) ((port==U1)?&U1STAT:&U2STAT)

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

    ringbuffer_create( &U1STAT.send_buffer, (void*)U1STAT.send_buffer_data, sizeof(char), UART_BUFFER_SIZE );
    U1STAT.rx_callback = 0;
    U1STAT.rx_newline_callback = 0;
    U1STAT.flags = 0;
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

    ringbuffer_create( &U1STAT.send_buffer, (void*)U1STAT.send_buffer_data, sizeof(char), UART_BUFFER_SIZE );
    U2STAT.rx_callback = 0;
    U2STAT.rx_newline_callback = 0;
    U2STAT.flags = 0;
}

void uart_set_flag(UARTPort port, UARTFlag flag, unsigned char value)
{
    UART_STATUS* stat = STAT(port);

    stat->flags &= ~(1 << flag);
    stat->flags |= (value & 0b1) << flag;
}

unsigned int uart_get_flag(UARTPort port, UARTFlag flag)
{
    UART_STATUS* stat = STAT(port);
    return ((stat->flags & (1<<flag)) != 0);
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

static void std_rx_callback(UARTPort port, unsigned char data);

static inline void std_rx_callback_task( UARTPort port ) 
{
    UART_STATUS* stat = STAT(port);

    stat->rx_newline_callback( (char*)stat->rx_linebuffer, stat->rx_linebuffer_cursor-stat->rx_linebuffer, stat->rx_linebuffer_remaining == 0 );
    // It is up to the caller to clear out the buffer.
}

static void std_rx_callback_task_U1() 
{
    std_rx_callback_task( U1 );
}

static void std_rx_callback_task_U2() 
{
    std_rx_callback_task( U2 );
}

static void std_rx_callback(UARTPort port, unsigned char data) 
{
    UART_STATUS* stat = STAT(port);

    /*
     * Allow certain characters to be processed in the interrupt to make
     * sure that they cannot be blocked by a noncooperative task that
     * never returns, so the taskloop very runs.
     * User can configure to allow resetting the device on a NULL character
     * as well as echoing 255.  This is useful when normal communication is
     * ASCII 7-bit so 0 and 255 will never be sent normally.
     */

    if (uart_get_flag(port, kResetOnNullCharacterFlag) && (data == 0))
        asm_reset();
    else if (uart_get_flag(port, kDiscardReceivedCharactersFlag))
        return;
    else if (uart_get_flag(port, kEnableHeartbeatFlag) && (data == kUARTHeartbeatCharacter))
    {
        put(port, data);
        return;
    }

    if (stat->rx_linebuffer_remaining <= 0)
        return;

    bool newline = (data=='\n');

    if (!newline) 
    {
        *(U1STAT.rx_linebuffer_cursor++) = data;
        --(stat->rx_linebuffer_remaining);
    }

    if ( newline || stat->rx_linebuffer_remaining == 0 ) 
        taskloop_add((port==U1)?std_rx_callback_task_U1:std_rx_callback_task_U2);
}

static void std_rx_callback_U1( char data ) 
{
    std_rx_callback(U1, data);
}
static void std_rx_callback_U2( char data ) 
{
    std_rx_callback(U2, data);
}

void uart_clear_receive_buffer(UARTPort port)
{
    UART_STATUS* stat = STAT(port);

    stat->rx_linebuffer_cursor = stat->rx_linebuffer;
    stat->rx_linebuffer_remaining = stat->rx_linebuffer_length;
}

void uart_set_newline_callback(UARTPort port, uart_newline_callback callback, volatile char *linebuffer, unsigned int buffer_length)
{
    UART_STATUS* stat = STAT(port);
    stat->rx_newline_callback = callback;
    stat->rx_linebuffer = linebuffer;
    stat->rx_linebuffer_length = buffer_length;

    uart_clear_receive_buffer(port);

    stat->rx_callback = (port==U1)?std_rx_callback_U1:std_rx_callback_U2;
}

void clear_uart_rx_newline_callback( UARTPort port ) 
{
    STAT(port)->rx_callback = 0;
    STAT(port)->rx_newline_callback = 0;
}

void process_RX_char(UART_STATUS* stat, char data) 
{
    if (stat->rx_callback) 
        stat->rx_callback(data);
}

//Interrupt Handlers
void __attribute__((interrupt,no_auto_psv)) _U1RXInterrupt()
{
   while(U1STAbits.URXDA == 1)
        process_RX_char(&U1STAT, U1RXREG);

    IFS0bits.U1RXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,auto_psv)) _U2RXInterrupt()
{
    while(U2STAbits.URXDA == 1)
        process_RX_char(&U2STAT, U2RXREG);

    IFS1bits.U2RXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U1TXInterrupt()
{
    while ( U1STAbits.UTXBF == 0 && !ringbuffer_empty( &U1STAT.send_buffer ) ) 
    {
        char data;
        ringbuffer_pop( &U1STAT.send_buffer, &data );
        U1TXREG = data;//data;
    }

    IFS0bits.U1TXIF = 0; //Clear IFS flag
}

void __attribute__((interrupt,no_auto_psv)) _U2TXInterrupt()
{
    while ( U2STAbits.UTXBF == 0 && !ringbuffer_empty( &U2STAT.send_buffer ) ) {
        char data;
        ringbuffer_pop( &U2STAT.send_buffer, &data );
        U2TXREG = data;
    }
    IFS1bits.U2TXIF = 0; //Clear IFS flag
}

void transmit_one( UARTPort port ) {
    UART_STATUS* stat = STAT( port );
    char data;
    ringbuffer_pop( &stat->send_buffer, &data );

    if (port == U1)
        U1TXREG = data;//data;
    else
        U2TXREG = data;
}

// IO Utility functions
void put(UARTPort port, const char c)
{
    ringbuffer_push( &STAT(port)->send_buffer, (void*)&c );
    transmit_one( port );
    wait_for_transmission(port);
}

void send(UARTPort port, const char *msg)
{
    if (strlen(msg) == 0)
        return;

    while ( *(msg) != '\0' ) {
        ringbuffer_push( &STAT(port)->send_buffer, (void*)msg );
        ++msg; //need to increment after passing to ringbuffer push.
    }
    transmit_one( port );
}

static inline void wait_for_transmission( UARTPort port ) {
    while ( !ringbuffer_empty( &STAT(port)->send_buffer ) )
        ;
    //Even after we stop filling the transmit FIFO, wait until the last bit is
    //shifted out.  Fixes a bug where the device reset command will not be
    //able to send the last 4 bytes of its message because the device resets
    //with those characters in the transmit shift register.
    if (port == U2) {
        while (U2STAbits.TRMT == 0) //TRMT = 0 when buffer and shift register empty
            ;
    } else {
        while (U1STAbits.TRMT == 0)
            ;
    }
}

void sends(UARTPort port, const char *msg)
{
    send(port, msg);
    wait_for_transmission( port );
}

void sendf(UARTPort port, const char *fmt, ...)
{
    UART_STATUS* stat = STAT( port );
    while ( !ringbuffer_empty( &stat->send_buffer ) )
        ;
    va_list argp;
    va_start(argp, fmt);

    // This is a hack, but it works.
    ringbuffer_reset( &stat->send_buffer );
    stat->send_buffer.end += sprintf_small((char*)stat->send_buffer_data, UART_BUFFER_SIZE, fmt, argp);

    va_end(argp);

    transmit_one( port );
    wait_for_transmission( port );
}

static void readln_callback_U1( char* buf, int len, bool overflown ) {
    clear_uart_rx_newline_callback( U1 );
}
static void readln_callback_U2( char* buf, int len, bool overflown ) {
    clear_uart_rx_newline_callback( U2 );
}
unsigned int readln_sync( UARTPort port, char* buffer, int buffer_length ) {
    uart_set_newline_callback( port, (port==U1)?readln_callback_U1:readln_callback_U2, buffer, buffer_length );
    while( STAT(port)->rx_newline_callback )
        ;
    return STAT(port)->rx_linebuffer_cursor - STAT(port)->rx_linebuffer;
}
