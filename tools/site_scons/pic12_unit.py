#pic12_unit.py
#Routines for building unit tests for the pic12 processor
from SCons.Script import *
from SCons.Environment import Environment
import os

def build_exec_unittest(test_files, chip, define):
	"""
	Build the unit test described by the source files test_files
	"""

	builddir = os.path.join('build', chip)
	testdir = os.path.join('build', 'test', chip)
	
	env = Environment(tools=['xc8_compiler', 'patch_mib12', 'merge_mib12_app'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	env['CHIP'] = chip
	env['CHIPDEFINE'] = define

	incs = []
	incs.append('.')
	incs.append('src')
	incs.append('src/mib')
	incs.append('../shared/pic12/src')
	incs.append('../shared/portable/core')
	incs.append('../shared/portable/mib')
	incs.append('../shared/portable/mib/slave')
	incs.append('../shared/portable/mib/master')
	incs.append('../shared/portable/util')

	env['INCLUDE'] = incs
	
	#Load in all of the xc8 configuration from build_settings
	mib12conf = utilities.MIB12Config()
	mib12conf.config_env_for_app(env)

	srcfiles = ['../shared/pic12/test/mib12_exec_unittest.c', '../shared/pic12/test/mib12_exec_unittest_startup.as']

	apphex = env.xc8(os.path.join(testdir, 'test_app.hex'), srcfiles)
	outhex = env.merge_mib12_app(os.path.join(testdir, 'test_combined.hex'), [os.path.join(builddir, 'mib12_executive_patched.hex'), apphex])
