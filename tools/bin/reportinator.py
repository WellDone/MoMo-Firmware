#!/usr/bin/env python

import sys, os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
from pymomo.commander.proxy import *

import cmdln

class Reportinator(cmdln.Cmdln):
	name = 'reportinator'

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_start(self, subcmd, opts):
		"""${cmd_name}: Start reporting

		${cmd_usage}
		${cmd_option_list}
		"""
		con = self._get_controller( opts )
		con.start_reporting()

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_stop(self, subcmd, opts):
		"""${cmd_name}: Stop reporting

		${cmd_usage}
		${cmd_option_list}
		"""
		con = self._get_controller( opts )
		con.stop_reporting()

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_send(self, subcmd, opts):
		"""${cmd_name}: Send a single report

		${cmd_usage}
		${cmd_option_list}
		"""
		con = self._get_controller( opts )
		con.send_report();

	def _get_controller(self,opts):
		try:
			con = get_controller(opts.port)
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

		return con

if __name__ == "__main__":
	reportinator = Reportinator()
	sys.exit(reportinator.main())