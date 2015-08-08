/*
 * config_manager.h
 * A persistent store of configuration variables that are pushed to submodules upon registration
 * allowing the modules to configure themselves automatically for each specific application.
 */

#ifndef __config_manager_h__
#define __config_manager_h__

#include <stdint.h>
#include "memory_manager.h"

#define kCMMagicNumber	0xFBAD
#define kCMMajorVersion	1
#define kCMMinorVersion 0
#define kCMPatchVersion 0

typedef enum
{
	kMatchOp_DontCare = 0,
	kMatchOp_Exact = 1,
	kMatchOp_GreaterEqual = 2
} CMMatchOperation;

typedef enum
{
	kCMNoError = 0,
	kCMInvalidEntryError,
	kCMObsoleteEntryError,
	kCMInvalidIndexError,
	kCMInvalidSequenceID,
	kCMInvalidEntryLength,
	kCMNoEntryInProgress,
	kCMOverrodePreviousEntryInProgress,
	kCMDataWouldOverflow
} CMErrorCode;

//9 bytes
typedef struct
{
	union
	{
		struct
		{
			char name[6];

			uint8_t major;
			uint8_t minor;
		};

		uint8_t uuid[8];
	};

	union
	{
		struct
		{
			uint8_t major_match_op : 3;
			uint8_t minor_match_op : 3;
			uint8_t name_match_op : 1;
			uint8_t uuid_match_op: 1;
		};

		uint8_t match_operations;
	};
} cm_match_data;

//16 bytes per cm_control_entry structure
typedef struct
{
	uint16_t magic_number;

	uint8_t  data[14];
} cm_generic_control_entry;

/*
 * Entry describing a configuration variable that should be pushed to matching modules
 * Modules may be matched based on a combination of their name and major.minor versions
 * or by exactly matching their 8 byte unique IDs.
 */

typedef struct
{
	uint16_t 		magic_number;
	uint16_t 		data_offset;
	uint16_t 		data_length;

	cm_match_data 	match_data;
	uint8_t 		valid;
} cm_info_control_entry;

typedef struct
{
	uint16_t 		magic_number;
	uint16_t		major_version;
	uint16_t		minor_version;
	uint16_t		patch_version;

	uint8_t 		reserved[8];
} cm_info_start_entry;

typedef struct
{
	cm_info_start_entry 	flash_header;
	uint16_t 				num_entries;

	uint32_t 				next_data_address;

	cm_info_control_entry 	entry_in_progress;
	uint16_t				sequence_number;
	uint16_t 				in_progress_flag;
} cm_state_data;

//API

void 		cm_init();
void 		cm_clear();

CMErrorCode cm_get_entry(uint16_t i, cm_info_control_entry *out);
CMErrorCode cm_start_entry(cm_match_data *match_info, uint16_t name, uint16_t *out_id);
CMErrorCode cm_finish_entry(uint16_t id);
CMErrorCode cm_add_data(uint16_t id, void *data, uint16_t length);

uint16_t 	cm_num_entries();

#endif
