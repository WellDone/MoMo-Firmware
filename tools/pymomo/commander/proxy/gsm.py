import proxy12
from pymomo.commander.exceptions import *
from pymomo.commander.types import *
from pymomo.commander.cmdstream import *
from pymomo.utilities.console import ProgressBar
import struct
from intelhex import IntelHex
from time import sleep

class GSMModule (proxy12.MIB12ProxyObject):
	def __init__(self, stream, addr):
		super(GSMModule, self).__init__(stream, addr)
		self.name = 'GSM Module'

	def power_module(self, on=True):
		"""
		Turn on power to the GSM module
		"""

		if on:
			self.rpc(10, 1)
		else:
			self.rpc(10, 6)

	def send_text(self, number, text):
		"""
		Send a text message to the given number which must have the form:
		+NUMBER with no dashes or spaces, for example: +16506695211
		"""

		print "Sending message '%s' to %s" % ( text, number )
		print "> start (%s)" % number
		self.rpc(11, 0, number)
		for i in xrange(0, len(text), 20):
			buf = text[i:i+20]
			print "> stream (%s)" % buf
			self.rpc(11, 1, buf)
		print "> end"
		self.rpc(11, 2)

	def module_on(self):
		res = self.rpc(10, 0, result_type=(1, False))

		if res['ints'][0] == 1:
			return True

		return False

	def module_off(self):
		res = self.rpc(10,5)
		return True

	def at_cmd(self, cmd, wait=0.5):
		"""
		Send an AT command to the GSM module and wait for its
		response.
		"""

		res = self.rpc(10, 2, cmd, result_type=(0, True))

		return res['buffer']

	def dump_buffer(self):
		res = self.rpc(10,4, result_type=(0, True))

		return res['buffer']

	def debug(self):
		res = self.rpc(10,7, result_type=(0, True))

		return res['buffer']		
