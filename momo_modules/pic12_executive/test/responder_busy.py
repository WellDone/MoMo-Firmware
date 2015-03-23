def handle_mib_endpoint(feature, command, type, sender, params):
	"""
	Always return that we're busy
	"""
	
	#-1 with no data indicates a busy response
	return -1, []