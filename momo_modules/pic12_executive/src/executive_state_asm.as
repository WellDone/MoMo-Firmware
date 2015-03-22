
global _mib_packet, _mib_state, _status

;space=1 is data memory
PSECT execdata,class=BANK1,space=1,noexec
_mib_packet: ds 25
_mib_packet_end:

_mib_state: ds 3
_mib_state_end:

_status: ds 2
_status_end