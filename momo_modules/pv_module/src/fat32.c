//fat32.c

#include "fat32.h"
#include "sdcard.h"

FAT32VolumeInfo vol;
uint8 			filename[11];

extern uint8 sector[512];

static uint32_t f32_nextcluster(uint32_t curr);

SDErrorCode f32_openvol()
{
	uint32_t part_start;
	uint8	 part_code;

	//Initialize file state
	vol.open_flags = kFileInvalid;
	vol.curr_sector = 0xFFFFFFFF;

	//Read in the MBR
	if (sd_read(0) != kSDNoError)
		return kSDIOError;

	//Make sure the MBR is valid
	if (sector[510] != 0x55 || sector[511] != 0xAA)
		return kSDInvalidMBR;

	//Make sure partition 1 is a FAT32 partition
	part_code = sector[kPartition1TypeOffset];
	if (part_code != 0x0B && part_code != 0x0C)
		return kSDInvalidPartition;

	part_start = *(uint32_t *)(sector + kPartition1LBAOffset);
	return f32_openpart(part_start);
}

SDErrorCode f32_openpart(uint32_t part_start)
{
	uint16_t reserved_sectors;
	uint32_t sectors_per_fat;

	if (sd_read(part_start) != kSDNoError)
		return kSDIOError;

	if (sector[0x10] != 2)
		return kSDInvalidPartition;

	reserved_sectors = *(uint16_t *)(sector + 0x0E);
	sectors_per_fat = *(uint32_t *)(sector + 0x24);

	vol.fat_start =  part_start + reserved_sectors;
	vol.cluster_start = vol.fat_start + (sectors_per_fat << 1); //num_fats * sectors_per_fat
	vol.cluster_size = sector[0x0D];
	vol.rootdir_cluster = *(uint32_t *)(sector + 0x2C);

	return kSDNoError;
}

uint32_t f32_clustertolba(uint32_t cluster)
{
	return vol.cluster_start + (cluster - 2)*vol.cluster_size;
}

SDErrorCode f32_loadsector(uint32_t sector)
{
	SDErrorCode code;
	if (sector == vol.curr_sector)
		return kSDNoError;

	
	code = sd_read(sector);
	if (code != kSDNoError)
	{
		vol.curr_sector = 0xFFFFFFFF;
		return code;
	}

	vol.curr_sector = sector;
	return kSDNoError;
}

SDErrorCode f32_sync()
{
	if (vol.curr_sector == 0xFFFFFFFF)
		return kSDInvalidSector;

	return sd_write(vol.curr_sector);
}

SDErrorCode f32_seek(uint32_t sector)
{
	uint32_t cluster_idx;
	uint32_t curr_cluster = vol.open_root_cluster;
	uint8_t  sector_offset;

	if (vol.open_flags == kFileInvalid)
		return kSDFileNotOpenError;

	if (sector >= (vol.open_size >> 9))
		return kSDEOFError;

	vol.open_sector = sector;

	cluster_idx = sector / vol.cluster_size;	//FIXME: can optimize this since vol.cluster_size is a power of 2 always

	while (cluster_idx-- > 0)
		curr_cluster = f32_nextcluster(curr_cluster);

	sector_offset = sector & (vol.cluster_size - 1);

	sector = f32_clustertolba(curr_cluster) + sector_offset;

	return f32_loadsector(sector);
}

static uint32_t f32_nextcluster(uint32_t curr)
{
	uint32_t fat_sector = vol.fat_start + (curr & kFATSectorMask);
	uint16_t  fat_index  = curr & kFATEntryMask;

	fat_index *= 4;

	f32_loadsector(fat_sector);
	
	return *(uint32_t*)(sector + fat_index);
}

SDErrorCode f32_findfile()
{
	uint32_t root_start = f32_clustertolba(vol.rootdir_cluster);
	uint8 	j, match;
	uint16  i;

	vol.open_flags = kFileInvalid;

	if (f32_loadsector(root_start) != kSDNoError)
		return kSDIOError;

	for (i=0; i<512; i+=32)
	{
		FAT32Entry *entry = (FAT32Entry *)(sector + i);

		//0xE5 indicated unused entry
		if (entry->name[0] == 0xE5)
			continue;

		//0x00 indicates end of directory
		if (entry->name[0] == 0x00)
			return kSDFileNotFoundError;

		//Long filename records have the low 4 bits set, ignore those
		if ((entry->attrib & 0b1111) == 0b1111)
			continue;

		match = 1;

		for (j=0; j<11; ++j)
		{
			if (entry->name[j] != filename[j])
			{
				match = 0;
				break;
			}
		}

		if (match == 1)
		{
			vol.open_root_cluster = entry->cluster_high;
			vol.open_root_cluster <<= 16;
			vol.open_root_cluster |= entry->cluster_low;
			vol.open_size = entry->size;
			vol.open_flags = kFileFound;

			return kSDNoError;
		}
	}

	return kSDFileNotFoundError;
}