#unit_test12.py

from unit_test import UnitTest
import pic12_unit

class Pic12UnitTest (UnitTest):
	def __init__(self, files):
		UnitTest.__init__(self, files)

	def _parse_target(self, target):
		return target

	def build_target(self, target, summary_env):
		cmdfile = None

		if hasattr(self, 'cmdfile'):
			cmdfile = self.cmdfile 
			
		pic12_unit.build_unittest(self.files, self.name, target, self.type, summary_env, cmds=cmdfile)
