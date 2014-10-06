#Automatic building of firmware and unit tests using the
#scons based momo build system

import utilities
import pic24
import pic12
import unit_test
import unit_test12
from SCons.Script import *

def autobuild_pic12(module, test_dir='test'):
	"""
	Build the given module for all targets and build all unit tests.
	targets are determined from /config/build_settings.json using
	the module name and the tests are found automatically and built 
	using their builtin metadata
	"""
	
	family = utilities.get_family('mib12')
	family.for_all_targets(module, lambda x: pic12.build_module(module, x))
	
	unit_test.build_units(test_dir, family.targets(module))

	Alias('release', os.path.join('build', 'output'))
	Alias('test', os.path.join('build', 'test', 'output'))
	Default('release')

def autobuild_pic24(module, test_dir='test', modulefile=None):
	"""
	Build the given pic24 module for all targets.
	"""

	family = utilities.get_family('mib24', modulefile)
	family.for_all_targets(module, lambda x: pic24.build_module(module, x))

	Alias('release', os.path.join('build', 'output'))
	Alias('test', os.path.join('build', 'test', 'output'))
	Default('release')
