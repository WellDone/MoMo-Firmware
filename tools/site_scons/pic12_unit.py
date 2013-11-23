#pic12_unit.py
#Routines for building unit tests for the pic12 processor
from SCons.Script import *
from SCons.Environment import Environment
import os
import utilities

def build_exec_unittest(test_files, name, chip):
	"""
	Build the unit test described by the source files <test_files>
	"""

	mib12conf = utilities.MIB12Config()

	dirs = mib12conf.build_dirs(chip)

	builddir = dirs['build']
	testdir = os.path.join(dirs['test'], name, 'objects')
	finaldir = dirs['output']
	outdir = os.path.join(dirs['test'], name)
	
	env = Environment(tools=['xc8_compiler', 'patch_mib12', 'merge_mib12_app', 'merge_mib12_sym'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')

	incs = []
	incs.append('.')
	incs.append('src')
	incs.append('src/mib')
	incs.append(testdir)

	env['INCLUDE'] = incs

	#Copy over the symbol file so we can reference it
	symfile = Command(os.path.join(testdir, 'mib12_executive_symbols.h'),  os.path.join(builddir, 'mib12_executive_symbols.h'), Copy("$TARGET", "$SOURCE"))
	execsymtab = Command(os.path.join(testdir, 'symbols.stb'),  os.path.join(builddir, 'mib12_executive_symbols.stb'), Copy("$TARGET", "$SOURCE"))

	symtab = env.merge_mib12_symbols([os.path.join(outdir, 'symbols.stb')], [execsymtab, os.path.join(testdir, name + '_app.sym')])

	#Load in all of the xc8 configuration from build_settings
	mib12conf.config_env_for_app(env, chip)
	alias,define,sim = mib12conf.find_chip_info(chip)

	env['TESTCHIP'] = sim
	env['TESTNAME'] = name

	srcfiles = test_files
	srcfiles += ['../test/pic12/exec_harness/mib12_exec_unittest.c', '../test/pic12/exec_harness/mib12_exec_unittest_startup.as', '../test/pic12/gpsim_logging/test_log.as']

	apphex = env.xc8(os.path.join(testdir, name + '_app.hex'), srcfiles)
	env.Depends(apphex[0], symfile)

	localexec = env.Command(os.path.join(testdir, 'mib12_executive_local.hex'), os.path.join(builddir, 'mib12_executive_patched.hex'), action='python ../../tools/scripts/patch_start.py $SOURCE $TARGET')
	outhex = env.merge_mib12_app(os.path.join(outdir, name + '.hex'), [localexec, apphex[0]])

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
		f.write('log w %s.ccpr1l\n' % sim)
		f.write('log w %s.ccpr1h\n' % sim)
		f.write('log on %s\n' % os.path.basename(str(target[1])))
		f.write('run\n')