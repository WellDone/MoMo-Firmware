#ifndef __test_log_h__
#define __test_log_h__

#define assert(value, expected)		do {FSR0L=value;assertv(expected);} while(0)

void assertv(unsigned char expected);

#endif