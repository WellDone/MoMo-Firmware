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
		v = self._average(sens, opts.n)

		print "Voltage: %d" % v

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	def do_offset(self, subcmd, opts, offset):
		"""${cmd_name}: Set the voltage offset of the digitally controlled amplifier
		from between 0 to 255, where 0 corresponds to VSS and 255 corresponds to VDD.

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = sens.set_offset(int(offset))
		print "Setting offset to %d" % int(offset)

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	def do_gain1(self, subcmd, opts, gain):
		"""${cmd_name}: Set the stage 1 gain of the amplifier between 0 and 127.

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = sens.set_stage1_gain(int(gain))
		print "Setting Stage 1 gain to: %d" % int(gain)

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	def do_gain2(self, subcmd, opts, gain):
		"""${cmd_name}: Set the stage 2 gain of the amplifier between 0 and 7.

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)
		v = sens.set_stage2_gain(int(gain))
		print "Setting Stage 2 gain to: %d" % int(gain)

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	@cmdln.option('-a', '--address', help='The MIB address of the multisensor module' )
	@cmdln.option('-n', '--average', type=int, default=1, help='Average for N samples' )

	def do_offset_bias(self, subcmd, opts):
		"""${cmd_name}: Determine the offset bias point by inverting the signal
		and measuring the average betwen the normal and inverted signals.

		${cmd_usage}
		${cmd_option_list}
		"""

		sens = self._create_proxy(opts)

		sens.set_inverted(False)
		sens.set_offset(127)
		vnorm = self._average(sens, opts.average)
		sens.set_inverted(True)
		vinv = self._average(sens, opts.average)

		print "Normal Voltage: %d" % vnorm
		print "Inverted Voltage: %d" % vinv
		print "Offset Bias: %d" % (vnorm + vinv)/2

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