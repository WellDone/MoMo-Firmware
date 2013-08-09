//mib_state.h
//Should not be included directly, only for use in bus.h

extern bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];		//Put mib state in first byte of GPR in bank1
extern bank1 MIBState 	mib_state;								//Put mib state right after mib_buffer