#ifndef __flash_memory_h__
#define __flash_memory_h__

extern void flash_erase_application();
extern void flash_write_row(uint8 row);
extern uint8 check_bootloader();
extern void flash_erase_row(uint8 row);
extern uint8 get_firmware_id();
#endif