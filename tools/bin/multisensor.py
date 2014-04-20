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
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	@cmdln.option('-n', '--average', type=int, default=1, help='Average for N samples' )
	def do_read(self, subcmd, opts):
		"""${cmd_name}: Read the voltage from the multisensor module

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = self._average(sens, opts.average)

		print "Voltage: %d" % v

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	def do_set(self, subcmd, opts, name, value):
		"""${cmd_name}: Set the parameter 'name' to the value 'value'

		Valid parameter names are: offset, gain1, gain2, select, invert and delay
		Valid values are: offset: [0, 255], gain1: [0,127], gain2: [0,7], 
		select: [current|differential], invert: [yes|no], delay: [1, 255]

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		sens.set_parameter(name, value)


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

	def _average(self, sens, n):
		readings = []
		for i in xrange(0, n):
			v = sens.read_voltage()
			readings.append(v)

		return sum(readings)/len(readings)

if __name__ == "__main__":
	sensortool = SensorTool()
	sys.exit(sensortool.main())