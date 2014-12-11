import intelhex

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

	if old is not None and instr != 0b101:
		print "Instruction was not a goto at address 0x%x" % address
		return False

	old_addr = (high & 0b111) << 8 | low

	if old is not None and old_addr != old:
		print "Unexpected address found (expected: 0x%x, found 0x%x)" % (old, old_addr)
		return False

	new_high = 0b101000 | ((new >> 8) & 0b111)
	new_low = new & 0xFF

	ih[address*2] = new_low
	ih[address*2 + 1] = new_high

	return True