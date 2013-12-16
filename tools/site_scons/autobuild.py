#Automatic building of firmware and unit tests using the
#scons based momo build system

import utilities
import unit_test
import unit_test12

def autobuild_pic12(module, test_dir='test'):
	"""
	Build the given module for all targets and build all unit tests.
	targets are determined from /config/build_settings.json using
	the module name and the tests are found automatically and built 
	using their builtin metadata
	"""
	targets = utilities.get_module_targets(module)

	utilities.for_all_targets(module, lambda x: utilities.build_app_for_chip(module, x))
	unit_test.build_units(test_dir, targets, subclass=unit_test12.Pic12UnitTest)
