my_address = 9

def execute_call():
	return (10, 0x0001, [])

def prepare_call():
	to_addr, command, params = execute_call()

	packet = [0]*24
	packet[0] = min(len(params), 20)
	packet[1] = my_address
	packet[2] = command & 0xFF
	packet[3] = (command >> 8) & 0xFF

	for i in xrange(0, len(params)):
		packet[4+i] = params[i]

	packet.append(_calculate_checksum(packet))

	return (my_address, to_addr, packet)

def process_response(response):
	resp = ", ".join(map(hex, response))
	print "[%s]" % resp

def _calculate_checksum(packet):
	cnt = 0

	for i in xrange(0, len(packet)):
		cnt += packet[i]

	checksum = 256 - (cnt & 0xFF)
	return checksum
