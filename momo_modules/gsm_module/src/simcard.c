//simcard.c

#include "simcard.h"

uint8 test_siminserted()
{
	RA4 = 1;
	TRISA4 = 0;
	TRISC3 = 1;
	if (RC3 == 0)
	{
		TRISA4 = 1;
		return 1;
	}

	TRISA4 = 0;
	return 0;
}