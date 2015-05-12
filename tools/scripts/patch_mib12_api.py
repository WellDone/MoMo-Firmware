#!/usr/bin/env python
#patch_mib12_api.py
#XC8 is terrible.  It cannot add a table of API callbacks without dying because it 
#gets confused about when the functions can be called and starts duplicating code like crazy.
#To work around that, it's necessary to patch the actual addresses for the api to call after
#compilation, which is what this script does

import intelhex
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.hex8 import patch

if len(sys.argv) < 5:
	print "usage: patch_mib12_api.py <api address> <source hex> <source symtab> <dest hex>"
	sys.exit(1)

api_addr = int(sys.argv[1])

if (api_addr % 16) != 0:
	print "MIB API was not aligned on a row boundary, failing.  Address was: %d" % api_addr
	sys.exit(1)

#Load in the symbol table
symtab = patch.parse_symtab(sys.argv[3])
main = symtab["_main"][0]

rpcbegin = symtab['_bus_master_begin_rpc'][0]
rpcsend = symtab["_bus_master_send_rpc"][0]
setreturn = symtab["_bus_slave_returndata"][0]
trap = symtab["_trap"][0]
reset = symtab["_exec_reset"][0]

ih = intelhex.IntelHex(sys.argv[2])

res4 = patch.patch_goto(ih, api_addr+11, main, reset)
res0 = patch.patch_goto(ih, api_addr+12, main, trap)
res1 = patch.patch_goto(ih, api_addr+13, main, rpcbegin)
res2 = patch.patch_goto(ih, api_addr+14, main, rpcsend)
res3 = patch.patch_goto(ih, api_addr+15, main, setreturn)

if res0 is False or res1 is False or res2 is False or res3 is False or res4 is False:
	sys.exit(1)

ih.write_hex_file(sys.argv[4])