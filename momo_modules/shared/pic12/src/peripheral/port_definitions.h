/*
 * port_definitions.h
 * For each known chip, define what functions are known for each port so that 
 * we can transparently support, for example, making pins analog if possible
 * or ensuring that pins are ANALOG when necessary.  We need to use a long list
 * because there is no way to procedurally generate the definitions using standard C macros.
 * 
 * NB. This file should never be included directly.  It is only for use with the associated port.h file.
 */

#ifndef __port_definitions_h__
#define __port_definitions_h__

#ifdef __PIC16LF1847__
#define ENSURE_DIGITAL_B0	
#define ENSURE_DIGITAL_B1	PIN_TYPE(B1, DIGITAL)
#define ENSURE_DIGITAL_B2	PIN_TYPE(B2, DIGITAL)
#define ENSURE_DIGITAL_B3	PIN_TYPE(B3, DIGITAL)
#define ENSURE_DIGITAL_B4	PIN_TYPE(B4, DIGITAL)
#define ENSURE_DIGITAL_B5	PIN_TYPE(B5, DIGITAL)
#define ENSURE_DIGITAL_B6	PIN_TYPE(B6, DIGITAL)
#define ENSURE_DIGITAL_B7	PIN_TYPE(B7, DIGITAL)

#define ENSURE_DIGITAL_A0	PIN_TYPE(A0, DIGITAL)
#define ENSURE_DIGITAL_A1	PIN_TYPE(A1, DIGITAL)
#define ENSURE_DIGITAL_A2	PIN_TYPE(A2, DIGITAL)
#define ENSURE_DIGITAL_A3	PIN_TYPE(A3, DIGITAL)
#define ENSURE_DIGITAL_A4	PIN_TYPE(A4, DIGITAL)
#define ENSURE_DIGITAL_A5	
#define ENSURE_DIGITAL_A6	
#define ENSURE_DIGITAL_A7	

#define TRY_ANALOG_B0	
#define TRY_ANALOG_B1	PIN_TYPE(B1, ANALOG)
#define TRY_ANALOG_B2	PIN_TYPE(B2, ANALOG)
#define TRY_ANALOG_B3	PIN_TYPE(B3, ANALOG)
#define TRY_ANALOG_B4	PIN_TYPE(B4, ANALOG)
#define TRY_ANALOG_B5	PIN_TYPE(B5, ANALOG)
#define TRY_ANALOG_B6	PIN_TYPE(B6, ANALOG)
#define TRY_ANALOG_B7	PIN_TYPE(B7, ANALOG)

#define TRY_ANALOG_A0	PIN_TYPE(A0, ANALOG)
#define TRY_ANALOG_A1	PIN_TYPE(A1, ANALOG)
#define TRY_ANALOG_A2	PIN_TYPE(A2, ANALOG)
#define TRY_ANALOG_A3	PIN_TYPE(A3, ANALOG)
#define TRY_ANALOG_A4	PIN_TYPE(A4, ANALOG)
#define TRY_ANALOG_A5	
#define TRY_ANALOG_A6	
#define TRY_ANALOG_A7	

#elif __PIC16LF1823__
#define ENSURE_DIGITAL_C0	PIN_TYPE(C0, DIGITAL)
#define ENSURE_DIGITAL_C1	PIN_TYPE(C1, DIGITAL)
#define ENSURE_DIGITAL_C2	PIN_TYPE(C2, DIGITAL)
#define ENSURE_DIGITAL_C3	PIN_TYPE(C3, DIGITAL)
#define ENSURE_DIGITAL_C4	
#define ENSURE_DIGITAL_C5	

#define ENSURE_DIGITAL_A0	PIN_TYPE(A0, DIGITAL)
#define ENSURE_DIGITAL_A1	PIN_TYPE(A1, DIGITAL)
#define ENSURE_DIGITAL_A2	PIN_TYPE(A2, DIGITAL)
#define ENSURE_DIGITAL_A3	
#define ENSURE_DIGITAL_A4	PIN_TYPE(A4, DIGITAL)
#define ENSURE_DIGITAL_A5	

#define TRY_ANALOG_C0	PIN_TYPE(C0, ANALOG)
#define TRY_ANALOG_C1	PIN_TYPE(C1, ANALOG)
#define TRY_ANALOG_C2	PIN_TYPE(C2, ANALOG)
#define TRY_ANALOG_C3	PIN_TYPE(C3, ANALOG)
#define TRY_ANALOG_C4	
#define TRY_ANALOG_C5	

#define TRY_ANALOG_A0	PIN_TYPE(A0, ANALOG)
#define TRY_ANALOG_A1	PIN_TYPE(A1, ANALOG)
#define TRY_ANALOG_A2	PIN_TYPE(A2, ANALOG)
#define TRY_ANALOG_A3	
#define TRY_ANALOG_A4	PIN_TYPE(A4, ANALOG)
#define TRY_ANALOG_A5	

#elif __PIC12LF1822__
#warning PIC12LF1822 does not have pin definitions yet

#else
#error Unknown Chip does not have pin definitions yet, check port_definitions.h
#endif

#endif