#ifndef __utilities_h__
#define __utilities_h__

#include <stdarg.h>
#include "common.h"

unsigned char get_2byte_number(char *input);
void sprintf_small(char *buffer, unsigned int len, char *fmt, va_list argp);
int itoa_small(char *buf, unsigned int len, int num);
bool atoi_small(char *buf, int* out);

#endif
