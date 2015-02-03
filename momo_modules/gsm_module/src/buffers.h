#ifndef __buffers_h__
#define __buffers_h__


#define GSM_RECEIVE_BUFFER_LENGTH	100
#define GSM_APN_LENGTH				21
#define GSM_COMM_DESTINATION_LENGTH	65

//Buffer IDs
#define kReceiveBuffer 			0
#define kAPNBuffer				1
#define kDestinationBuffer		2

#ifndef DEFINES_ONLY
#include <xc.h>
#include <stdint.h>

extern char gsm_rx_buffer[GSM_RECEIVE_BUFFER_LENGTH];
extern uint8 rx_buffer_start;
extern uint8 rx_buffer_end;
extern uint8 rx_buffer_len;

extern char gprs_apn[GSM_APN_LENGTH];


void reset_expected1_ptr();
void reset_expected2_ptr();

uint8 check_inc_expected1(uint8 current);
uint8 check_inc_expected2(uint8 current);

void load_buffer();
#endif

#endif