#compare_hex.py

import intelhex
import sys

def print_diff(standard, test):
	valid_addr = standard.addresses()

	
	for addr in valid_addr:
		if standard[addr] != test[addr]:
			a = addr / 2
			high = addr % 2

			if high:
				print "Difference at 0x%X, high byte (wanted 0x%X, was 0x%X)" % (a, standard[addr], test[addr])
			else:
				print "Difference at 0x%X, low byte (wanted 0x%X, was 0x%X)" % (a, standard[addr], test[addr])

standard = intelhex.IntelHex(sys.argv[2])
test = intelhex.IntelHex(sys.argv[1])

print_diff(standard, test)