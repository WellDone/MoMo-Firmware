
global _mib_data,_mib_state
;space=1 is data memory
PSECT absdata,abs,ovrld,space=1,delta=1
org 0xA0
_mib_data: ds 25
_mib_data_end:
_mib_state: ds 3
_mib_state_end: