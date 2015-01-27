#ifndef __flash_memory_h__
#define __flash_memory_h__

extern void flash_erase_executive();
extern void flash_write_row(uint8 row);
extern void flash_erase_row(uint8 row);

extern void copy_mib_to_boot(uint8 dst_addr);
extern void load_boot_address();

extern uint8 verify();
#endif