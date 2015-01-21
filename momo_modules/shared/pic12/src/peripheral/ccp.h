#ifndef __ccp_h__
#define __ccp_h__

enum
{
	kCCPOff = 0b0000,
	kCCPCompare = 0b0010,
	kCCPCaptureFallingEdge = 0b0100,
	kCCPCaptureRisingEdge = 0b0101,
	kCCPCapture4thRisingEdge = 0b0110,
	kCCPCapture16thRisingEdge = 0b0111,
	kCCPCompareInitHigh = 0b1000,
	kCCPCompareInitLow = 0b1001,
	kCCPCompareInterruptOnly = 0b1010,
	kCCPCompareSpecial = 0b1011
};

#define pack_ccp_config(mode)		(mode & 0b1111)
#define ccp_register(n)				CCP ## n ## CON
#define ccp_reglow_r(n)				CCPR ## n ## L
#define ccp_reghigh_r(n)			CCPR ## n ## H
#define ccp_ie_r(n)					CCP ## n ## IE
#define ccp_if_r(n)					CCP ## n ## IF

#define ccp_config(n, mode)			ccp_register(n) = pack_ccp_config(mode)
#define ccp_off(n)					ccp_config(n, kCCPOff)
#define ccp_getlow(n)				ccp_reglow_r(n)				
#define ccp_gethigh(n)				ccp_reghigh_r(n)
#define ccp_get(n)					((((uint16_t)ccp_gethigh(n)) << 8) | ccp_getlow(n))

#define ccp_set_ie(n, val)			ccp_ie_r(n) = val
#define ccp_set_if(n, val)			ccp_if_r(n) = val
#define ccp_if(n)					ccp_if_r(n)

#endif