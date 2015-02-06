#ifndef __memory_h__
#define __memory_h__

#include <stdio.h>
#include <stdlib.h>
#include "pic24.h"

#ifndef __NO_FLASH__

#define MEMORY_SUBSECTION_MASK 0xFFFULL
#define MEMORY_SUBSECTION_SIZE 0x1000ULL

#define MEMORY_SUBS_PER_SECTION 16


#define MEMORY_SUBSECTION_ADDR(num) (MEMORY_SUBSECTION_SIZE*(num))
#define MEMORY_ADDR_SUBSECTION(addr) ( (addr)>>12 )
#define MEMORY_SUBSECTION_OFFSET(addr) ( (addr) & MEMORY_SUBSECTION_MASK )
#define MEMORY_ADDR_SUBSECTION_ADDR(addr) ( (addr) & ~MEMORY_SUBSECTION_MASK )

//Convert a sector number to a subsector number (sector size=64K, subsector size=4K, 16 subsectors per sector)
#define MEMORY_SECTION_TO_SUB(sec)	(((unsigned long long)(sec)) << 4)
#define MEMORY_SECTION_ADDR(sec) 	MEMORY_SUBSECTION_ADDR(MEMORY_SECTION_TO_SUB(sec))

#define MEMORY_ADDRESS_MASK 0xFFFFFL
#define MEMORY_CAPACITY			0x14		//defined in M25PX_80 datasheet (page 21)

typedef struct
{
	unsigned int write_wait : 1;
	unsigned int enabled : 1;
	unsigned int reserved : 14;
} memory_config;

typedef enum
{
	kReadOnly = 0,
	kReadWrite = 1
} MemoryStartupTimer;

void mem_write(uint32 addr, const BYTE* data, unsigned int length );
void mem_write_aligned(const uint32 addr, const BYTE* data, unsigned int length);
void mem_read(uint32 addr, BYTE* buf, unsigned int numBytes );

void mem_clear_subsection(uint32 addr);
void mem_clear_all();

BYTE mem_status();
bool mem_test();

void mem_remove_power();
void mem_ensure_powered(MemoryStartupTimer for_writing);
void mem_init();
unsigned int mem_enabled();

#endif
#endif
