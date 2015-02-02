#ifndef __buffers_h__
#define __buffers_h__


#define GSM_RECEIVE_BUFFER_LENGTH	100

//Buffer IDs
#define kReceiveBuffer 0

#ifndef DEFINES_ONLY
#include <xc.h>
#include <stdint.h>

extern char gsm_rx_buffer[GSM_RECEIVE_BUFFER_LENGTH];
extern uint8 rx_buffer_start;
extern uint8 rx_buffer_end;
extern uint8 rx_buffer_len;

void reset_expected1_ptr();
void reset_expected2_ptr();

uint8 check_inc_expected1(uint8 current);
uint8 check_inc_expected2(uint8 current);
#endif

#endif