import proxy12
from pymomo.commander.exceptions import *
from pymomo.commander.types import *
from pymomo.commander.cmdstream import *
from pymomo.utilities.console import ProgressBar
import struct
from intelhex import IntelHex
from time import sleep

class MultiSensorModule (proxy12.MIB12ProxyObject):
	ranges = {'offset': set(range(0, 256)), 'gain1': set(range(0, 128)), 'gain2':set(range(0,8)), 'delay': set(range(1, 256)), 
				'invert': set(['yes', 'no']), 'select':set(['current', 'differential'])}
	mapper = {'yes': 1, 'no': 0, 'current': 1, 'differential': 0}
	ids = {'offset': 0, 'gain1': 1, 'gain2': 2, 'select': 3, 'invert': 4, 'delay': 5}
	integral = set(['offset', 'gain1', 'gain2', 'delay'])

	def __init__(self, stream, addr):
		super(MultiSensorModule, self).__init__(stream, addr)
		self.name = 'MultiSensor Module'

	def set_parameter(self, name, value):
		if name not in self.ranges:
			raise KeyError("Invalid parameter options are: %s" % str(self.ranges.keys()))

		if name in self.integral:
			value = int(value)

		valid = self.ranges[name]
		if value not in valid:
			raise ValueError("Invalid parameter value for key %s" % name)

		if not isinstance(value, int):
			if value not in self.mapper:
				raise ValueError("Invalid parameter value for key %s, cannot be mapped to a known integer" % name)

			value = self.mapper[value]

		self.rpc(20, 3, self.ids[name], value)

	def read_voltage(self):
		res = self.rpc(20, 5, result_type=(1,False))
		return res['ints'][0]

	def acquire_pulses(self):
		self.rpc(20, 6)

	def read_pulses(self):
		res = self.rpc(20, 7, result_type=(1, False))
		return res['ints'][0]