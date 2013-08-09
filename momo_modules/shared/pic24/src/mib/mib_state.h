//mib_state.h

//Should not be included directly, only for use in bus.h

extern unsigned char 	mib_buffer[kBusMaxMessageSize];		//Put mib state in first byte of GPR in bank1
extern MIBState 		mib_state;							//Put mib state right after mib_buffer