#!/usr/bin/env python

import sys, os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
from pymomo.commander.proxy import *

import cmdln

class GSMTool(cmdln.Cmdln):
	name = 'gsmtool'

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_on(self, subcmd, opts):
		"""${cmd_name}: Turn on the GSM modem.

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		gsm.module_on()

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_off(self, subcmd, opts):
		"""${cmd_name}: Turn off the GSM modem

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		gsm.module_off()

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_dump(self, subcmd, opts):
		"""${cmd_name}: Dump the modem's serial buffer (for debugging)

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		res = gsm.dump_buffer()
		print "buffer (%d): %s" % (len(res), res)
		for i in range( 0, len(res) ):
			print ord(res[i])

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_debug(self, subcmd, opts):
		"""${cmd_name}: Get debug info

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		res = gsm.debug()
		print "module_on: %s" % bool(ord(res[0]))
		print "shutdown_pending: %s" % bool(ord(res[1]))
		print "rx_buffer_start: %d" % ord(res[2])
		print "rx_buffer_end: %d" % ord(res[3])
		print "rx_buffer_len: %d" % ord(res[4])

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_cmd(self, subcmd, opts, cmd):
		"""${cmd_name}: Tell the GSM modem to execute an AT command

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		res = gsm.at_cmd( cmd )
		print res

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_msg(self, subcmd, opts, dest, text ):
		"""${cmd_name}: Send a text message with the content <text> to <dest>

		${cmd_usage}
		${cmd_option_list}
		"""
		gsm = self._create_proxy( opts )
		#gsm.module_on();
		res = gsm.send_text( dest, text );
		#gsm.module_off();

	def _create_proxy(self,opts):
		try:
			con = get_controller(opts.port)
			address = 11
			if opts.address != None:
				address = opts.address
			gsm = GSMModule(con.stream, address)
		except NoSerialConnectionException as e:
			print "Available serial ports:"
			if not e.available_ports():
				print "<none>"
			else:
				for port, desc, hwid in e.available_ports():
					print "\t%s (%s, %s)" % ( port, desc, hwid )
			self.error(str(e))
		except ValueError as e:
			self.error(str(e))

		return gsm

if __name__ == "__main__":
	gsmtool = GSMTool()
	sys.exit(gsmtool.main())