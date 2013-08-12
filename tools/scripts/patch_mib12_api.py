#patch_mib12_api.py
#XC8 is terrible.  It cannot add a table of API callbacks without dying because it 
#gets confused about when the functions can be called and starts duplicating code like crazy.
#To work around that, it's necessary to patch the actual addresses for the api to call after
#compilation, which is what this script does

import intelhex
import sys

def parse_symtab(path):
	lines = [line.strip() for line in open(path)]

	symtab = {}

	for line in lines:
		if line == "%segments":	
			break

		fields = line.split(' ')

		if len(fields) != 5:
			raise ValueError("File did not have a valid format, too many fields in line:\n%s\n" % line)

		name = fields[0]
		addr = int(fields[1], 16)
		sect = fields[3]

		symtab[name] = (addr, sect)

	return symtab

def patch_goto(ih, address, old, new):
	"""
	Given an address in words (2 bytes), make sure that it points to 
	a goto instruction pointing to the old address and if so update
	it to point to the new address.
	"""

	low = ih[address*2]
	high = ih[address*2  + 1]

	instr = (high & 0b111000) >> 3

	if instr != 0b101:
		print "Instruction was not a goto at address 0x%x" % address
		return False

	old_addr = (high & 0b111) << 8 | low

	if old_addr != old:
		print "Unexpected address found (expected: 0x%x, found 0x%x)" % (old, old_addr)
		return False

	new_high = 0b101000 | ((new >> 8) & 0b111)
	new_low = new & 0xFF

	ih[address*2] = new_low
	ih[address*2 + 1] = new_high

	return True

#Load in the symbol table
symtab = parse_symtab(sys.argv[2])
main = symtab["_main"][0]
rpc = symtab["_bus_master_rpc_sync"][0]
setreturn = symtab["_bus_slave_setreturn"][0]

ih = intelhex.IntelHex(sys.argv[1])

patch_goto(ih, 16*78+14, main, rpc)
patch_goto(ih, 16*78+15, main, setreturn)

ih.write_hex_file(sys.argv[3])