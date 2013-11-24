#unit_test12.py

from unit_test import UnitTest
from utilities import MIB12Config
import pic12_unit

class Pic12UnitTest (UnitTest):
	def __init__(self, files):
		self.conf = MIB12Config()		#must come before __init__ to superclass since that calls _parse_target
		UnitTest.__init__(self, files)

	def _parse_target(self, target):
		return self.conf.get_chip_name(target)

	def build_target(self, target):
		pic12_unit.build_unittest(self.files, self.name, target, self.type)