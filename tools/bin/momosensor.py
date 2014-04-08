import sys
import os.path
import os
from random import randint

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
import cmdln

import sched, time

class MoMoSensor(cmdln.Cmdln):
	name = 'MoMoSensor'

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_emulate(self, subcmd, opts):
		"""${cmd_name}: Log a random (1-1000) sensor value every .1 seconds

		${cmd_usage}
		${cmd_option_list}
		"""
		
		con = self._get_controller(opts)

		s = sched.scheduler(time.time, time.sleep)
		def log_event( scheduler ):
			s.enter( .1, 1, log_event, (scheduler,) )
			value = randint(1,1000)
			con.log_sensor_event(value)
			print "Logged event!  Value: %d" % value

		s.enter( 1, 1, log_event, (s,) )
		s.run()

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_readone(self, subcmd, opts):
		"""${cmd_name}: Read one event from the log.

		${cmd_usage}
		${cmd_option_list}
		"""
		
		con = self._get_controller(opts)
		event = con.read_sensor_value()

		print "Stream: %d" % event.stream
		print "MetaData: %d" % event.metadata
		print "Timestamp: {:%Y-%m-%d %H:%M:%S}".format( event.timestamp )
		print "Value: %d" % event.value

	@cmdln.option('-p', '--port', help='Serial port that fsu is plugged into')
	def do_log(self, subcmd, opts, value):
		"""${cmd_name}: Log one integer value (max: 2^64)

		${cmd_usage}
		${cmd_option_list}
		"""
		con = self._get_controller(opts)
		con.log_sensor_event(int(value))
		print "Logged event!"

	def _get_controller(self, opts):
		try:
			c = get_controller(opts.port)
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

		return c

if __name__ == "__main__":
	emulator = MoMoSensor()
	sys.exit(emulator.main())