import sys
import os.path
from pymomo.utilities import build

class MIBHandler:
	"""
	A class representing a MIB callback routine suitable for processing those found in mib blocks
	pulled from hex files or for creating those blocks based on descriptions of the handlers.
	"""
	BufferSizeMask = 0b11111
	BufferFlag = 1<<7

	def __init__(self, spec, symbol=None, address=None):
		self.symbol = symbol
		self.address = address
		self.spec = spec

		self._validate()

	@classmethod
	def Create(cls, symbol, ints=0, has_buffer=False):
		if ints > 4:
			raise ValueError("Invalid MIBHandler Spec: too many int parameters. (value = %d"  % ints)
		if ints < 0:
			raise ValueError("Invalid MIBHandler Spec: negative number of int parameters. (value = %d)" % ints)

		spec = 0

		if has_buffer:
			spec = 1 << 7

		spec |= (ints&0b11) << 5

		return MIBHandler(spec, symbol)

	def num_ints(self):
		return (self.spec >> 5) & 0b11

	def has_buffer(self):
		return self.spec & MIBHandler.BufferFlag

	def _validate(self):
		if self.symbol is None and self.address is None:
			raise ValueError("You must pass either a symbol or an address to MIBHandler")

	def _format_spec(self):
		ints = self.num_ints()
		buffer = self.has_buffer()

		if buffer and (ints==0):
			return "buffer only"
		elif buffer:
			return "%d integers and a buffer" % ints
		elif (ints==0) and not buffer:
			return "no parameters"

		return "%d integers" % ints

	def __str__(self):
		name = "<unnamed> "
		addr = "<no address>"

		if self.symbol is not None:
			name = self.symbol + " "

		if self.address is not None:
			addr = "0x%X" % self.address

		return "%s@ %s (%s)" % (name, addr, self._format_spec())