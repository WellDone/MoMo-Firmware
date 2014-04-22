#!/usr/bin/env python

import sys, os

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.commander.meta import *
from pymomo.commander.exceptions import *
from pymomo.commander.proxy import *

import cmdln
import struct
import base64

def chunk(l, n):
    for i in xrange(0, len(l), n):
        yield l[i:i+n]

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

	def do_parse(self, subcmd, opts, report):
		"""${cmd_name}: Parse a report in BASE64 format

		${cmd_usage}
		${cmd_option_list}
		"""

		agg_names = ['count','sum','mean','<none>','<none>','min','max']
		interval_types = ['second', 'minute', 'hour', 'day']
		report = base64.b64decode(report)

		if ord(report[0]) != 2:
			print "Unrecognized report version %d" % ord(report[0])
		sensor, sequence, flags, battery_voltage, diag1, diag2, bulk_aggs, int_aggs, interval_def, interval_count = struct.unpack( "xBHHHHHBBBB", report[:16] )

		interval_type = interval_def & 0xF
		interval_step = interval_def >> 4
		if interval_type > len(interval_types):
			interval_type = "<unknown:%d>" % interval_type
		else:
			interval_type = interval_types[interval_type]

		bulk_agg_names = []
		for i in range(0,7):
			if ( bulk_aggs & (0x1 << i) ):
				bulk_agg_names.append(agg_names[i])

		int_agg_names = []
		for i in range(0,7):
			if ( int_aggs & (0x1 << i) ):
				int_agg_names.append(agg_names[i])

		print "version: 2"
		print "sensor: %d" % sensor
		print "sequence: %d" % sequence
		print "flags: %d" % flags
		print "battery charge: %d%%" % ((battery_voltage / 1024.0)*100);
		print "diag: %d, %d" % (diag1, diag2)
		print "Interval type: %s" % interval_type
		print "         step: %d" % interval_step
		print "        count: %d" % interval_count

		if bulk_aggs != 0:
			print "Global aggregates:"
			values = struct.unpack( "H" * len(bulk_agg_names), report[16:16+len(bulk_agg_names)*2] )
			for i in range( len(values) ):
				print "\t%s = %d" % (bulk_agg_names[i], values[i])

		if int_aggs != 0:
			print "Interval Aggregates:"
			values = struct.unpack( "H" * len(int_agg_names) * interval_count, report[16+len(bulk_agg_names)*2:] )
			row_format ="{:>8}" * (len(int_agg_names) + 1)
			print row_format.format("", *int_agg_names)
			for row in chunk(values, len(int_agg_names)):
				print row_format.format("", *row)



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