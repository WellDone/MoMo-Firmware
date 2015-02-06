;startup.as

#include <xc.inc>

jumpm MACRO name,dest
name:
	pagesel(dest)
	goto  dest && ((1<<11) - 1)
ENDM

global _task,_initialize,_interrupt_handler,start,intlevel1

PSECT reset_vec,global,class=CODE,delta=2
start:
goto doinit
goto dointer
goto dotask

jumpm doinit,_initialize
jumpm dointer,_interrupt_handler
jumpm dotask,_task

intlevel1:

PSECT powerup,global,class=CODE,delta=2

PSECT functab,global,class=CODE,delta=2

PSECT config,global,class=CODE,delta=2

PSECT idloc,global,class=CODE,delta=2

PSECT eeprom_data,global,class=CODE,delta=2