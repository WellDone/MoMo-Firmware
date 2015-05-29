#Define well-known exception types
class BusyException (Exception):
	pass

def handle_mib_endpoint(command, sender, params):
	"""
	Always return that we're busy
	"""
	
	raise BusyException
