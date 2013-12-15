#verify_hex.py

import intelhex
import sys

def print_ranges(hexfile):
	valid_addr = ih.addresses()

	rstart = valid_addr[0]

	for i in xrange(2, len(valid_addr)/2):
		if valid_addr[2*i] != valid_addr[2*i - 2] + 2:
			rend = valid_addr[2*i - 2]
			print "Address Range: 0x%X-0x%X" % (rstart/2, rend/2)

			rstart = valid_addr[2*i]

ih = intelhex.IntelHex(sys.argv[1])
ih.padding = 0xFF

print_ranges(ih)