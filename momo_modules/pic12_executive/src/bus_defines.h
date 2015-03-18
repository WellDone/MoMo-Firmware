#ifndef __bus_defines_h__
#define __bus_defines_h__

//Bus error codes that can be returned
#define kNoMIBError 				0
#define kUnsupportedCommand 		1
#define kWrongParameterType 		2
#define kParameterTooLong 			3
#define kChecksumError				4
#define kUnknownError 				6
#define kCallbackError 				7
#define kSlaveNotAvailable 			255

//Other Constants
#define kInvalidMIBIndex			255
#define kBusMaxMessageSize 			20

#define kBusHeaderSize				4
#define kBusPacketSize				(kBusHeaderSize + kBusMaxMessageSize)

#endif
