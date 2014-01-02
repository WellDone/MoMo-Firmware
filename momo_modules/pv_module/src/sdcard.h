//sdcard.h

#ifndef __sdcard_h__
#define __sdcard_h__

#include "platform.h"
#include <stdint.h>

#define SD_TIMEOUT		250

//SD Card Response Types
typedef struct
{
	uint8 in_idle_state :1;
	uint8 erase_reset: 1;
	uint8 illegal_cmd: 1;
	uint8 crc_error: 1;
	uint8 erase_seq_error: 1;
	uint8 address_error: 1;
	uint8 param_error: 1;
	uint8 start_bit: 1;
} SDResponseByte1;

typedef struct
{
	uint8 card_locked :1;
	uint8 wp_erase_skip: 1;
	uint8 unspecified_error: 1;
	uint8 controller_error: 1;
	uint8 ecc_failed: 1;
	uint8 wp_violation: 1;
	uint8 erase_parameter: 1;
	uint8 out_of_range: 1;
} SDResponseByte2;

typedef struct
{
	SDResponseByte1 b1;
	SDResponseByte2 b2;
	uint8 			ocr_reg[3];
} SDResponse;

typedef enum 
{
	kSDTypeR1 = 1,
	kSDTypeR2 = 2,
	kSDTypeR3 = 5
} SDResponseType;

typedef enum
{
	kSDNoError = 0,
	kSDInvalidMBR = 1,
	kSDInvalidPartition = 2,
	kSDIOError = 3,
	kSDFileNotFoundError = 4,
	kSDFileNotOpenError = 5,
	kSDInvalidSector = 6,
	kSDUnknownError = 1<<7,
	kSDTimeoutError = 0xFF
} SDErrorCode;

typedef enum
{
	kSDWriteOperation = 0,
	kSDReadOperation = 1
} SDTransferOperation;


void 		sd_pins_idle();
uint8 		sd_power_on();
void 		sd_power_off();
uint8 		sd_check_inserted();

SDErrorCode sd_write(uint32_t block);
SDErrorCode sd_read(uint32_t block);

SDErrorCode sd_initialize();


#endif