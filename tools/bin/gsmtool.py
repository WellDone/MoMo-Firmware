#!/usr/bin/env python

import sys, os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
from pymomo.commander.proxy import *
from time import sleep

if len(sys.argv) < 2:
	print "Command me!  available commands:\r\non, off, cmd, dump, msg"
	quit(1)

cmd = sys.argv[1]

con = get_controller('/dev/ttyUSB0')
try:
	gsm = GSMModule(con.stream, 11)

	if cmd == "on":
		gsm.module_on();
	if cmd == "off":
		gsm.module_off();
	elif cmd == "cmd":
		if len(sys.argv) != 3:
			print "No cmd specified"
			quit(1)
		res = gsm.at_cmd( sys.argv[2] )
		print res
	elif cmd == "dump":
		res = gsm.dump_buffer();
		print "buffer: " + res
	elif cmd == "msg":
		if len(sys.argv) != 4:
			print "USAGE: gsmtool msg <number> <text>"
			quit(1)
		#gsm.module_on();
		res = gsm.send_text( sys.argv[2], sys.argv[3] );
		#gsm.module_off();
except RPCException as e:
	print str(e)