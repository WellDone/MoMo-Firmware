#merge_hex.py
#Simple utility to merge two hex files and output the merged result.  Where
#the files overlap, the second hex file dominates.

import intelhex
import sys

if len(sys.argv) < 4:
	print "usage: merge_hex <file 1> <file 2> <output>"

orig = intelhex.IntelHex(sys.argv[1])
new = intelhex.IntelHex(sys.argv[2])

orig.merge(new, overlap='replace')

orig.tofile(sys.argv[3], format="hex")