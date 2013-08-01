;startup.as

#include <xc.inc>


global _main,start

PSECT reset_vec,global,class=CODE,delta=2
start:
goto _main

PSECT powerup,global,class=CODE,delta=2

PSECT functab,global,class=CODE,delta=2

PSECT config,global,class=CODE,delta=2

PSECT idloc,global,class=CODE,delta=2

PSECT eeprom_data,global,class=CODE,delta=2