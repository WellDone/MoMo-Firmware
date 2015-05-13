#Define well-known exception types
class BusyException (Exception):
	pass

class EndpointNotFoundException (Exception):
	pass

class ChecksumMismatchException (Exception):
	pass

def handle_mib_endpoint(command, sender, params):
	"""
	Always that we are doing this asynchronously
	"""
	
	return (0x3f, [])
