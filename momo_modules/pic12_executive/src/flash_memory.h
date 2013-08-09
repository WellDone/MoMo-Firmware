#ifndef __flash_memory_h__
#define __flash_memory_h__

extern void flash_erase_application();
extern void flash_write_row(uint8 row);
extern void flash_erase_row(uint8 row);

extern uint8 get_boot_source();
extern uint8 get_firmware_id();
extern void copy_mib_to_boot(uint8 dst_addr);
extern void load_boot_address();
#endif