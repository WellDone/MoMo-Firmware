#ifndef __controller_mib_feature_h
#define __controller_mib_feature_h

#define _BOOTLOADER_VAR  __attribute__((section(".bootvar")))
#define kReflashMagic	0xABCD

void con_init();
void con_reset_bus();
void con_enter_safe_mode();

#endif