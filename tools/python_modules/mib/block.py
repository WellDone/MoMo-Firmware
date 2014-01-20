#block.py
#An object representing a MIBBlock

import sys
import os.path
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from momo_utilities import build
from hex8.decode import *
import intelhex

block_size = 16

hw_offset = 0
type_offset = 1
info_offset = 2
name_offset = 3
num_feature_offset = 10
features_offset = 11
cmds_offset = 12
specs_offset = 13
handlers_offset = 14
magic_offset = 15

class MIBBlock:
	"""
	The block in program memory describing a MoMo application module.
	"""

	def __init__(self, ih):
		"""
		Given an intelhex object, extract the MIB block information
		from it or raise an exception if a MIBBlock cannot be found
		at the right location.
		"""

		build_settings = build.load_settings()

		self.base_addr = build_settings["mib12"]["mib"]["base_address"]
		self.curr_version = build_settings["mib12"]["mib"]["current_version"]

		self._load_from_hex(ih)

		self._check_consistency()

	def _check_magic(self, ih):
		magic_addr = self.base_addr + magic_offset
		instr = ih[magic_addr]

		#Last instruction should be retlw 0xAA for the magic number
		if instr == 0x34AA:
			return True

		return False

	def _check_consistency(self):
		"""
		Check that the features, commands, handlers, and specifications are mutually consistent
		"""

		if len(self.cmds) != len(self.features)+1:
			raise ValueError("Command list has wrong number of entries, was %d, should be %d" % (len(cmds), len(features)+1))

		if len(self.specs) != len(self.handlers):
			raise ValueError("There should be a 1-1 mapping between command specs and handlers. Their lengths differed (specs: %d, handlers: %d)." % (len(self.specs), len(self.handlers)))

		if len(self.handlers) != self.cmds[-1]:
			raise ValueError("Command array has invalid format.  Last entry should be the number of total handlers (total handlers: %d, cmd entry: %d)" % (len(self.handlers), self.cmds[-1]))

		return True

	def _load_from_hex(self, ih):
		if not self._check_magic(ih):
			raise ValueError("Invalid magic number.")

		self.num_features = decode_retlw(ih, self.base_addr + num_feature_offset)
		self.features_addr = decode_goto(ih, self.base_addr + features_offset)
		self.cmds_addr = decode_goto(ih, self.base_addr + cmds_offset)
		self.specs_addr = decode_goto(ih, self.base_addr + specs_offset)
		self.handlers_addr = decode_goto(ih, self.base_addr + handlers_offset)

		self.features = decode_table(ih, self.features_addr, decode_retlw)
		self.cmds = decode_table(ih, self.cmds_addr, decode_retlw)
		self.specs = decode_table(ih, self.specs_addr, decode_retlw)
		self.handlers = decode_table(ih, self.handlers_addr, decode_goto)

		self.name = decode_string(ih, self.base_addr + name_offset, 7)
		self.hw_type = decode_retlw(ih, self.base_addr + hw_offset)
		self.info = decode_retlw(ih, self.base_addr + info_offset)
		self.module_type = decode_retlw(ih, self.base_addr + type_offset)


ih = intelhex.IntelHex16bit('/home/timburke/src/welldone/MoMo-Firmware/momo_modules/gsm_module/build/output/gsm_module_16lf1823.hex')

try:
	block = MIBBlock(ih)
except ValueError as e:
	print e

print block.num_features
print block.features
print block.cmds
print block.handlers
print block.specs
print block.name
print block.module_type