
#include "config_manager.h"
#include "system_log.h"
#include "log_definitions.h"
#include <string.h>

cm_state_data cm_state;

//Internal Functions
static uint16_t cm_count_entries();


void cm_init()
{
	cm_info_control_entry last_entry;

	mem_read(MEMORY_SUBSECTION_ADDR(kConfigManagerControlSubsector), (BYTE*)&cm_state.flash_header, sizeof(cm_state.flash_header));

	if (cm_state.flash_header.magic_number != kCMMagicNumber)
	{
		if (cm_state.flash_header.magic_number != 0xFFFF)
			LOG_CRITICAL(kCMOverwritingUnknownData);

		cm_clear();
	}

	if (cm_state.flash_header.major_version != kCMMajorVersion || cm_state.flash_header.minor_version != kCMMinorVersion)
	{
		LOG_CRITICAL(kCMClearingOldConfigData);
		LOG_INT(cm_state.flash_header.major_version);
		LOG_INT(cm_state.flash_header.minor_version);
		cm_clear();
	}

	cm_state.num_entries = cm_count_entries();

	if (cm_state.num_entries == 0)
		cm_state.next_data_address = MEMORY_SUBSECTION_ADDR(kConfigManagerDataSubsector);
	else
	{
		cm_get_entry(cm_state.num_entries, &last_entry);
		cm_state.next_data_address = MEMORY_SUBSECTION_ADDR(kConfigManagerDataSubsector) + (uint32_t)last_entry.data_offset + (uint32_t)last_entry.data_length;
	}

	cm_state.sequence_number = 0;
	cm_state.in_progress_flag = 0;
}

void cm_clear()
{
	mem_clear_subsection(MEMORY_SUBSECTION_ADDR(kConfigManagerControlSubsector));
	mem_clear_subsection(MEMORY_SUBSECTION_ADDR(kConfigManagerDataSubsector));

	cm_state.flash_header.magic_number = kCMMagicNumber;
	cm_state.flash_header.major_version = kCMMajorVersion;
	cm_state.flash_header.minor_version = kCMMinorVersion;
	cm_state.flash_header.patch_version = kCMPatchVersion;

	mem_write_aligned(MEMORY_SUBSECTION_ADDR(kConfigManagerControlSubsector), (BYTE *)&cm_state.flash_header, sizeof(cm_state.flash_header));

	cm_state.num_entries = 0;
	cm_state.next_data_address = MEMORY_SUBSECTION_ADDR(kConfigManagerDataSubsector);
	cm_state.sequence_number = 0;
}

CMErrorCode cm_get_entry(uint16_t i, cm_info_control_entry *out)
{
	uint32_t address = MEMORY_SUBSECTION_ADDR(kConfigManagerControlSubsector) + ((uint32_t)i) * sizeof(cm_info_control_entry);

	if (i > cm_state.num_entries || i == 0)
		return kCMInvalidIndexError;

	mem_read(address, (BYTE*)out, sizeof(cm_info_control_entry));

	if (out->magic_number != kCMMagicNumber)
		return kCMInvalidEntryError;
	else if (out->valid != 0xFF)
		return kCMObsoleteEntryError;

	return kCMNoError;
}

CMErrorCode cm_start_entry(cm_match_data *match_info, uint16_t name, uint16_t *out_id)
{
	CMErrorCode error = kCMNoError;
	memcpy(&cm_state.entry_in_progress.match_data, match_info, sizeof(cm_match_data));

	cm_state.entry_in_progress.data_offset = cm_state.next_data_address;
	cm_state.entry_in_progress.data_length = 0;
	cm_state.entry_in_progress.valid = 0xFF;

	cm_state.sequence_number += 1;
	*out_id = cm_state.sequence_number;

	//Let our caller know that they trampled on someone else, but allow it since we don't
	//want to keep failing forever once there's one entry left in progress and the sequence
	//number will let us avoid any corruption.
	if (cm_state.in_progress_flag != 0)
		error = kCMOverrodePreviousEntryInProgress;

	cm_state.in_progress_flag = 1;

	//Push the name of this entry to the data as the first 2 bytes before the actual value.
	cm_add_data(cm_state.sequence_number, &name, sizeof(uint16_t));

	return error;
}

CMErrorCode cm_finish_entry(uint16_t id)
{
	uint32_t addr;

	if (cm_state.in_progress_flag == 0)
		return kCMNoEntryInProgress;

	if (id != cm_state.sequence_number)
		return kCMInvalidSequenceID;

	cm_state.in_progress_flag = 0;

	//The first two bytes are taken up by the name of the config variable, if there's no more data then this variable is useless
	if (cm_state.entry_in_progress.data_length == 2)
		return kCMInvalidEntryLength;

	addr = MEMORY_SUBSECTION_ADDR(kConfigManagerControlSubsector) + ((uint32_t)cm_state.num_entries + 1)*sizeof(cm_info_control_entry);
	mem_write_aligned(addr, (BYTE*)&cm_state.entry_in_progress, sizeof(cm_info_control_entry));

	cm_state.num_entries += 1;
	return kCMNoError;
}

CMErrorCode cm_add_data(uint16_t id, void *data, uint16_t length)
{
	if (cm_state.in_progress_flag == 0)
		return kCMNoEntryInProgress;

	if (id != cm_state.sequence_number)
		return kCMInvalidSequenceID;

	if ((cm_state.next_data_address + (uint32_t)length) >= MEMORY_SUBSECTION_ADDR(kConfigManagerDataSubsector + 1))
		return kCMDataWouldOverflow;

	mem_write(cm_state.next_data_address, (BYTE *)data, length);

	cm_state.next_data_address += length;
	cm_state.entry_in_progress.data_length += length;

	return kCMNoError;
}

static uint16_t cm_count_entries()
{
	uint16_t i = 1;
	cm_info_control_entry out;

	while (cm_get_entry(i, &out) == kCMNoError)
		++i;

	return i-1;
}

uint16_t cm_num_entries()
{
	return cm_state.num_entries;
}