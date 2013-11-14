#pic12_unit.py
#Routines for building unit tests for the pic12 processor
from SCons.Script import *
from SCons.Environment import Environment
import os
import utilities

def build_exec_unittest(test_files, name, chip):
	"""
	Build the unit test described by the source files test_files
	"""

	builddir = os.path.join('build', chip)
	testdir = os.path.join('build', 'test', chip, name, 'objects')
	outdir = os.path.join('build', 'test', chip, name)
	
	env = Environment(tools=['xc8_compiler', 'patch_mib12', 'merge_mib12_app'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')

	incs = []
	incs.append('.')
	incs.append('src')
	incs.append('src/mib')

	env['INCLUDE'] = incs
	
	#Load in all of the xc8 configuration from build_settings
	mib12conf = utilities.MIB12Config()
	mib12conf.config_env_for_app(env, chip)
	alias,define,sim = mib12conf.find_chip_info(chip)

	env['TESTCHIP'] = sim
	env['TESTNAME'] = name

	srcfiles = ['../shared/pic12/test/mib12_exec_unittest.c', '../shared/pic12/test/mib12_exec_unittest_startup.as']

	apphex = env.xc8(os.path.join(testdir, name + '_app.hex'), srcfiles)
	outhex = env.merge_mib12_app(os.path.join(outdir, name + '.hex'), [os.path.join(builddir, 'mib12_executive_patched.hex'), apphex[0]])

	outscript = env.Command([os.path.join(outdir, 'test.stc'), os.path.join(outdir, 'test.log')], outhex, action=build_unittest_script)

	#Also remember to remove the test directory when cleaning
	env.Clean(outscript, testdir)
	env.Clean(outscript, outdir)

def build_unittest_script(source, target, env):
	"""
	Build a gpsim script to execute this unit test
	"""

	sim = env['TESTCHIP']
	name = env['TESTNAME']

	with open(str(target[0]), "w") as f:
		f.write('processor %s\n' % sim)
		f.write('load s %s\n' % os.path.basename(str(source[0])))
		f.write('break w 0x291, reg(0x291) == 0x00\n')
		f.write('log w p12f1822.ccpr1l\n')
		f.write('log w p12f1822.ccpr1h\n')
		f.write('log on %s\n' % os.path.basename(str(target[1])))
		f.write('run\n')

	return 0
