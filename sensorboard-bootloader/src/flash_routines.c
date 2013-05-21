#include <htc.h>
#include <pic12f1822.h>

//****************************************************************
//  FLASH MEMORY READ
//  needs 16 bit address pointer in address
//  returns 14 bit value from selected address
//
//****************************************************************
unsigned int flash_memory_read (unsigned int address)
{

    EEADRL = ((address) & 0xff);
    EEADRH = ((address) >> 8);
    CFGS = 0;                   // access FLASH program, not config
    LWLO = 0;                   // only load latches

    EEPGD = 1;
    RD = 1;
    #asm
        NOP
        NOP
    #endasm
    return (EEDATH << 8) | EEDATL;
}

//****************************************************************
//  FLASH MEMORY WRITE
//  needs 16 bit address pointer in address, 16 bit data pointer
//
//****************************************************************
void flash_memory_write (unsigned int address, unsigned char *data )
{
    unsigned char wdi;

    EECON1 = 0;

    EEADRL = ((address) & 0xff);    // load address
    EEADRH = ((address) >> 8);      // load address

    for (wdi = 0; wdi < 14; wdi += 2)
    {
        EEDATH = data[wdi];
        EEDATL = data[wdi + 1];

        EEPGD = 1;                  // access program space FLASH memory
        CFGS = 0;                   // access FLASH program, not config
        WREN = 1;                   // allow program/erase
        LWLO = 1;                   // only load latches
        EECON2 = 0x55;
        EECON2 = 0xAA;


        WR = 1;                     // set WR to begin write
        #asm
            NOP
            NOP
        #endasm

        EEADR++;
    }

    EEDATH = data[14];
    EEDATL = data[15];
    EEPGD = 1;                  // access program space FLASH memory
    CFGS = 0;                   // access FLASH program, not config
    WREN = 1;                   // allow program/erase

    LWLO = 0;                   // this time start write
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1;                     // set WR to begin write
    #asm
        NOP
        NOP
    #endasm

    WREN = 0;                   // disallow program/erase
}

//****************************************************************
//  FLASH MEMORY ERASE
//  Program memory can only be erased by rows.
//  A row consists of 32 words where the EEADRL<4:0> = 0000.
//
//****************************************************************
void flash_memory_erase (unsigned int address)
{
    EEADRL=((address) & 0xff);    // load address
    EEADRH=((address) >> 8);      // load address
    CFGS = 0;                   // access FLASH program, not config
    WREN = 1;                   // allow program/erase
    EEPGD = 1;                  // access program space FLASH memory
    FREE = 1;                   // perform an erase on next WR command, cleared by hardware
    EECON2 = 0x55;              // required sequence
    EECON2 = 0xAA;              // required sequence
    WR = 1;                     // set WR to begin erase cycle
    WREN = 0;                   // disallow program/erase
}
