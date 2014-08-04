from unit_test import UnitTest, find_sources
import pic24
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from pymomo.utilities.typedargs.exceptions import *
from pymomo.utilities import paths
class Pic24ModuleTest (UnitTest):
	"""
	A unit test for the PIC24 series of microprocessors that tests a single .c/.h module
	providing mocks for any other modules that the module under test (MUT) references.
	"""

	def __init__(self, files):
		UnitTest.__init__(self, files)

		self.includes = []

		if self._find_module() == False:
			found = False
			for src in self.additional_sources:
				found = self._find_module(src)
				if found:
					break

			if not found:
				raise InternalError("Could not find module for module unit test %s" % self.name)

		self._add_harness()
	
	def _find_module(self, srcdir=None):
		testfile = os.path.basename(self.files[0])
		if not testfile.startswith('test_'):
			raise InternalError('Module unit test does not have an appropriate filename: %s' % testfile)

		if srcdir is None:
			testdir = os.path.dirname(self.files[0])
			srcdir = os.path.abspath(os.path.join(testdir, '..', 'src'))

		incdirs, sources, headers = find_sources(srcdir)
		self.includes += incdirs

		modname = os.path.splitext(testfile)[0][5:]
		if modname not in sources:
			return False

		self.files.append(sources[modname])
		return True

	def _add_harness(self):
		moddir = paths.MomoPaths().modules
		harnessdir = os.path.join(moddir, 'test', 'pic24')
		incdirs, sources, headers = find_sources(harnessdir)

		self.includes += incdirs
		self.files += sources.values()

	def _parse_target(self, target):
		return target

	def build_target(self, target, summary_env):
		chip = ChipFamily('mib24').
		pic24.build_moduletest(self.)
		pass	
