#compute_app_checksum.py
#Compute the checksum of the application portion of memory
#to verify correct programming of the pic12/pic16

import intelhex
import sys

ih = intelhex.IntelHex(sys.argv[1])
ih.padding = 0xFF

start_row = 44
start_addr = start_row*32

end_addr = 4096

check = 0
for i in xrange(start_addr, end_addr):
	low = ih[i*2]
	high = ih[i*2+1]

	high = high & 0b00111111

	check += low + high

	check &= 0xFF

check = check & 0xFF

print "Checksum: 0x%X" % check
print "Checksum: %d" % check
print "Checksum: %s" % bin(check)