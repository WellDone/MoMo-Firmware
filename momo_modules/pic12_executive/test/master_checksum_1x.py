# Make a slave endpoint call with an incorrect checksum 1 time
# and then send the correct checksum the next time to make sure
# that the checksum validation works correctly.

my_address = 9
num_calls = 0

def execute_call():
	return (10, 0x0a00, [])

def prepare_call():
	global num_calls

	to_addr, command, params = execute_call()

	packet = [0]*24
	packet[0] = min(len(params), 20)
	packet[1] = my_address
	packet[2] = command & 0xFF
	packet[3] = (command >> 8) & 0xFF

	for i in xrange(0, len(params)):
		packet[4+i] = params[i]

	packet.append(_calculate_checksum(packet))

	print "Preparing Call Data, num_calls=%d" % num_calls

	return (my_address, to_addr, packet)

def _calculate_checksum(packet):
	global num_calls

	cnt = 0

	for i in xrange(0, len(packet)):
		cnt += packet[i]

	checksum = 256 - (cnt & 0xFF)

	if num_calls == 0:
		num_calls += 1
		return checksum + 5

	return checksum
