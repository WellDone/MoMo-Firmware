import struct

class fwscheduler:
	intervals = {0: '0.5 seconds', 1: '1 second', 2: '10 second', 3: '1 minute', 4: '10 minutes', 5: '1 hour', 6: '1 day'}

	def __init__(self, buf):
		if len(buf) != size():
			raise ValueError("Invalid length, expected 6 got %d" % len(buf))

		ptr05s, ptr1s, ptr10s, ptr1m, ptr10m, ptr1h, ptr1d, freq = struct.unpack("<HHHHHHHH", buf)
		self.ptrs = [ptr05s, ptr1s, ptr10s, ptr1m, ptr10m, ptr1h, ptr1d]
		self.interval = freq

	def __str__(self):
		return default_formatter(self)

def size():
	return (7*2) + 2

def convert(arg):
	return fwscheduler(arg)

#Formatting Functions
def default_formatter(arg, **kwargs):
	out = ""
	out += "Scheduler Interval: %s\n" % arg.intervals[arg.interval]
	out += "Interval Buckets\n"

	for i in xrange(0, 7):
		name = arg.intervals[i]
		value = arg.ptrs[i]

		out += "%s: 0x%X\n" % (name, value)

	return out