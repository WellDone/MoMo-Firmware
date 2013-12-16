#!/usr/bin/python
#patch_start.py
#Overwrite the goto at address 0 with a goto to a fixed address for skipping executive code
#during unit tests

import intelhex
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python_modules'))
import hex8.patch

if len(sys.argv) < 4:
	print "usage: patch_start <new address> <file in> <file out>"
	sys.exit(1)

addr = int(sys.argv[1])

ih = intelhex.IntelHex(sys.argv[2])

res1 = hex8.patch.patch_goto(ih, 0, None, addr)

if res1 is False:
	sys.exit(1)

ih.write_hex_file(sys.argv[3])