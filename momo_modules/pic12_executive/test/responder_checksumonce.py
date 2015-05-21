#Define well-known exception types
class BusyException (Exception):
	pass

class EndpointNotFoundException (Exception):
	pass

class ChecksumMismatchException (Exception):
	pass

first_call = True

def handle_mib_endpoint(command, sender, params):
	"""
	Always return that we're busy
	"""

	global first_call

	if first_call:
		first_call = False
		raise ChecksumMismatchException()

	return 0x00, [x for x in range(0, 20)]
