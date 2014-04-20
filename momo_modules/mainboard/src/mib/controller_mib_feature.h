#ifndef __controller_mib_feature_h
#define __controller_mib_feature_h

#define _BOOTLOADER_VAR  __attribute__((section(".bootvar")))
#define kReflashMagic	0xABCD

#define MAX_MODULES 32


void con_init();
void con_reset_bus();

#endif