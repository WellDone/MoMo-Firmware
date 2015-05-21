
def handle_mib_endpoint(command, sender, params):
	"""
	Handle the MIB endpoint for the controller network address command.
	"""
	print "Command was: 0x%X" % command
	print "Sender was: 0x%X" % sender	
	return 0x00, [x for x in range(0, 20)]
