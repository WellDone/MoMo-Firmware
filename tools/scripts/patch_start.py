#!/usr/bin/python
#patch_start.py
#Overwrite the goto at address 0 with a goto to a fixed address for skipping executive code
#during unit tests

import intelhex
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python'))
import hex8.patch

ih = intelhex.IntelHex(sys.argv[1])

res1 = hex8.patch.patch_goto(ih, 0, None, 1266)

if res1 is False:
	sys.exit(1)

ih.write_hex_file(sys.argv[2])