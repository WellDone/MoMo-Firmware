import struct

class fwtask:
	def __init__(self, buf):
		if len(buf) != 6:
			raise ValueError("Invalid length, expected 6 got %d" % len(buf))

		addr, arg, crit = struct.unpack("<HHH", buf)
		self.addr = addr
		self.arg = arg
		self.critical = crit != 0

	def __str__(self):
		return default_formatter(self)

def size():
	return 6

def convert(arg):
	return fwtask(arg)

#Formatting Functions
def default_formatter(arg, **kwargs):
	return "Task Structure\n  Callback address: 0x%X\n  Argument: 0x%X\n  Critical: %s" % (arg.addr, arg.arg, str(arg.critical))

def format_address(arg, **kwargs):
	return "0x%X" % arg.addr