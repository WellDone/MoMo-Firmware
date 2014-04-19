#!/usr/bin/env python

import sys, os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
from pymomo.commander.proxy import *

import cmdln

class SensorTool(cmdln.Cmdln):
	name = 'multisensor'

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_read(self, subcmd, opts):
		"""${cmd_name}: Read the voltage from the multisensor module

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = sens.read_voltage()

		print "Voltage: %d" % v

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the GSM module' )
	def do_offset(self, subcmd, opts, offset):
		"""${cmd_name}: Set the voltage offset of the digitally controlled amplifier
		from between 0 to 255, where 0 corresponds to VSS and 255 corresponds to VDD.

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = sens.set_offset(int(offset))

	def _create_proxy(self,opts):
		try:
			con = get_controller(opts.port)
			address = 11
			if opts.address != None:
				address = opts.address
			gsm = MultiSensorModule(con.stream, address)
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
	sensortool = SensorTool()
	sys.exit(sensortool.main())