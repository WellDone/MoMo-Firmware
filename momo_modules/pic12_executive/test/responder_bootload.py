#Define well-known exception types
class BusyException (Exception):
	pass

class EndpointNotFoundException (Exception):
	pass

class ChecksumMismatchException (Exception):
	pass

def handle_mib_endpoint(command, sender, params):
	"""
	Return random 16 byte test pattern to check whether
	bootloading flash works
	"""

	global rom
	baseaddr = params[3] << 8 | params[2]

	retval = []
	for i in xrange(0, 16):
		addr = baseaddr + i
		retval.append(ord(rom[addr]))

	return 0x00, retval

#Load in the rom
rompath = 'support_bootload.bin'
with open(rompath, "rb") as f: #Need the binary flag for this to work cross platform
	rom = f.read()