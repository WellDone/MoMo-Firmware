#unit_test12.py

import unit_test
import pic12_unit

class Pic12UnitTest (unit_test.UnitTest):
	def __init__(self, files):
		unit_test.UnitTest.__init__(self, files)

	def _parse_target(self, target):
		return target

	def build_target(self, target, summary_env):
		cmdfile = None

		if hasattr(self, 'cmdfile'):
			cmdfile = self.cmdfile 
			
		pic12_unit.build_unittest(self.files, self.name, target, self.type, summary_env, cmds=cmdfile)

unit_test.known_types['executive'] = Pic12UnitTest
unit_test.known_types['application'] = Pic12UnitTest