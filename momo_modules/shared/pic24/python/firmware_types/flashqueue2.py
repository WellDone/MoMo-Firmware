import struct

class flashqueue2:
	def __init__(self, buf):
		if len(buf) != size():
			raise ValueError("Invalid length, expected 28 got %d" % len(buf))

		elemsize, shift, start_addr, length, magic, fbsector, fbcurrent, fbversion, fbitemsize, fbshift, start, count = struct.unpack("<BBLLHHHHBBLL", buf)
		self.element_size = elemsize
		self.element_shift = shift
		self.start_index = start
		self.count = count
		self.length = length
		self.start_address = start_addr
		self.fb_magic = magic
		self.fb_itemsize = fbitemsize
		self.fb_itemshift = fbshift

	def __str__(self):
		return default_formatter(self)

def size():
	return 28

def convert(arg):
	return flashqueue2(arg)

#Formatting Functions
def default_formatter(arg, **kwargs):
	out = "* Flashqueue v2 *\n"

	out += "Start Address: 0x%X\n" % arg.start_address
	out += "Size: %d\n" % arg.element_size
	out += "Rounded Size: %d\n" % 2** (arg.element_shift)
	out += "Start: %d\n" % arg.start_index
	out += "Count: %d\n" % arg.count
	out += "Length: %d\n" % arg.length
	out += "Valid Flashblock Magic: %s\n" % str(arg.fb_magic == 0xFABC)
	out += "  - Flashblock Item Size: %d\n" % arg.fb_itemsize
	out += "  - Flashblock Rounded Item Size: %d" % (2 ** arg.fb_itemshift)

	return out