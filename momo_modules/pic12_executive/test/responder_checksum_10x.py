#Define well-known exception types
class BusyException (Exception):
	pass

class EndpointNotFoundException (Exception):
	pass

class ChecksumMismatchException (Exception):
	pass

calls = 0

def handle_mib_endpoint(command, sender, params):
	"""
	Always return that we're busy
	"""

	global calls

	if calls < 10:
		calls += 1
		raise ChecksumMismatchException()

	return 0x00, [x for x in range(0, 20)]
