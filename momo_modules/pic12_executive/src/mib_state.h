//mib_state.h
//Should not be included directly, only for use in bus.h

//4 byte return value header
typedef struct
{
	unsigned char			status;
	unsigned char			checksum;
	unsigned char			reversed; //must be 0
	unsigned char			length;

} MIBReturnStatus;

//25 Bytes
typedef struct
{
	union
	{
		MIBCommandPacket 	bus_command;	//4 bytes
		MIBReturnStatus		return_status;	//4 bytes
	};
	unsigned char 			buffer[kBusMaxMessageSize]; //20 bytes
	unsigned char			checksum;
} MIBData;

extern bank1 MIBData			mib_data;							//Put mib data in first byte of GPR in bank1 (24 bytes)
extern bank1 MIBState 			mib_state;							//Put mib state right after mib_buffer (2 bytes)
