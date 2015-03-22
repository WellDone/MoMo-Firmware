//mib_state.h
//Should not be included directly, only for use in bus.h

typedef struct
{
	unsigned char 			reserved;
	MIBReturnValueHeader	bus_returnstatus;
	unsigned char			status_checksum;
} MIBReturnStatus;

//24 Bytes
typedef struct
{
	union
	{
		MIBCommandPacket 	bus_command;	//3 bytes
		MIBReturnStatus		return_status;	//3 bytes
	};
	unsigned char 			buffer[kBusMaxMessageSize]; //20 bytes
	unsigned char			checksum;
} MIBData;

extern MIBData			mib_data;							//Put mib data in first byte of GPR in bank1 (24 bytes)
extern bank1 MIBState 			mib_state;							//Put mib state right after mib_buffer (2 bytes)
