import struct

class scheduledtask:
	def __init__(self, buf):
		if len(buf) != size():
			raise ValueError("Invalid length, expected %d got %d" % (size(), len(buf)))

		callback,arg,flags,remaining,task_list,nextptr = struct.unpack("<HHBBHH", buf)
		
		self.callback = callback
		self.arg = arg
		self.remaining = remaining
		self.task_list = task_list
		self.next = nextptr

		self.schedule_forever = False
		self.currently_scheduled = False
		self.callback_pending = False

		if flags & (1 << 7):
			self.schedule_forever = True
		if flags & (1 << 6):
			self.currently_scheduled = True
		if flags & (1 << 5):
			self.callback_pending = True

	def __str__(self):
		return default_formatter(self)

def size():
	return 10

def convert(arg):
	return scheduledtask(arg)

def default_formatter(arg, **kwargs):
	out = "*Scheduled Task*\n"
	out += "  Callback: 0x%X\n" % arg.callback
	out += "  Argument: 0x%X\n" % arg.arg
	out += "  Remaining Calls: %d\n" % arg.remaining
	out += "  Flags\n"
	out += "    - Schedule Forever: %s\n" % str(arg.schedule_forever)
	out += "    - Currently Scheduled: %s\n" % str(arg.currently_scheduled)
	out += "    - Callback Pending: %s\n" % str(arg.callback_pending)
	return out