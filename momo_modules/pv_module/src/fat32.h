/*fat32.h
 * A very simple fat32 driver with support for 1 directory and a fixed set of files
 * that are preallocated.
 */

#ifndef __fat32_h__
#define __fat32_h__

#include "platform.h"
#include "sdcard.h"
#include <stdint.h>

#define kBootCodeSize			446
#define kPartition1TypeOffset	(kBootCodeSize + 4)
#define kPartition1LBAOffset	(kBootCodeSize + 8)
#define kFATEntryMask			(0b1111111)
#define kFATSectorMask			(~kFATEntryMask)

enum
{
	kFileFound = 0,
	kFileOpen = 1,
	kFileInvalid
};

typedef struct
{
	uint32_t fat_start;
	uint32_t cluster_start;
	uint32_t rootdir_cluster;
	uint8_t  cluster_size;

	uint32_t curr_sector;

	uint8 	 open_name[11];
	uint8 	 open_flags;
	uint32_t open_root_cluster;
	uint32_t open_sector;
} FAT32VolumeInfo;

typedef struct
{
	uint8  		name[11];
	uint8 		attrib;
	uint8 		reserved1[8];
	uint16_t	cluster_high;
	uint8 		reserved2[4];
	uint16_t 	cluster_low;
	uint32_t	size;
} FAT32Entry;

SDErrorCode f32_openvol();
SDErrorCode f32_openpart(uint32_t part_start);
SDErrorCode	f32_findfile();
SDErrorCode f32_loadsector(uint32_t sector);
SDErrorCode f32_seek(uint32_t sector);
SDErrorCode f32_sync();

uint32_t 	f32_clustertolba(uint32_t cluster);
#endif