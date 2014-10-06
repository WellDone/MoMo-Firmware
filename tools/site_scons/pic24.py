from SCons.Script import *
from SCons.Environment import Environment
import sys
import os.path
from utilities import BufferedSpawn

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from pymomo.utilities.paths import MomoPaths
from pymomo.sim.simulator import Simulator

all_envs = []

def build_module(module_name, chip):
	"""
	Configure Scons to build a hex module for the pic24 chip listed in the argument.
	"""

	dirs = chip.build_dirs()
	output_name = '%s.elf' % (chip.output_name(),)
	output_hex = '%s.hex' % (chip.output_name(),)

	VariantDir(dirs['build'], 'src', duplicate=0)

	env = Environment(tools=['xc16_compiler', 'xc16_assembler', 'xc16_linker'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	env['ARCH'] = chip
	env['OUTPUT'] = output_name

	#spawn = BufferedSpawn(env, 'test.log')

	#env['SPAWN'] = spawn.spawn
	Export('env')

	SConscript(os.path.join(dirs['build'], 'SConscript'))

	elffile = os.path.join(dirs['build'], output_name)
	hexfile = os.path.join(dirs['build'], output_hex)

	env.Command(hexfile, elffile, 'xc16-bin2hex %s' % elffile)
	
	output = env.InstallAs(os.path.join(dirs['output'], output_hex), hexfile)
	return [output]

def build_library(name, chip):
	"""
	Build the pic24 shared library for the given chip returning an absolute
	path to the product.  
	Parameters are:
	- name: name of the shared library
	- chip: a ChipSettings object for the target chip.
	"""

	dirs = chip.build_dirs()

	libdir = os.path.join(MomoPaths().modules, 'shared', 'pic24')
	builddir = os.path.join(libdir, dirs['build'])
	outdir = os.path.join(libdir, dirs['output'])

	output_name = '%s.a' % (chip.output_name(),)

	VariantDir(builddir, 'src', duplicate=0)

	library_env = Environment(tools=['xc16_compiler', 'xc16_assembler', 'xc16_linker'], ENV = os.environ)
	library_env.AppendENVPath('PATH','../../tools/scripts')
	library_env['ARCH'] = chip
	library_env['OUTPUT'] = output_name
	
	SConscript(os.path.join(builddir, 'SConscript'), exports='library_env')

	libfile = library_env.InstallAs(os.path.join(outdir, output_name), os.path.join(builddir, output_name))
	return os.path.join(outdir, output_name)

def build_moduletest(test, arch):
	"""
	Given a path to the source files, build a unit test including the unity test harness targeting
	the given architecture.
	"""

	arch = arch.retarget(add=['test'])

	build_dirs = test.build_dirs(arch)
	objdir = build_dirs['objects']

	unit_env = Environment(tools=['xc16_compiler', 'xc16_assembler', 'xc16_linker'], ENV = os.environ)
	unit_env['ARCH'] = arch
	unit_env['OUTPUT'] = '%s.elf' % test.name

	objs = []
	for src in test.files + arch.extra_sources():
		name,ext = os.path.splitext(os.path.basename(src))
		target = os.path.join(objdir, name + '.o')
		objs.append(unit_env.xc16_gcc('#' + target, src))

	unit_env.xc16_ld(os.path.join('#' + build_dirs['test'], unit_env['OUTPUT']), objs)
	#unit_env.Command(['#'])

# def run_unit_test(test_elf, arch, logfile, statusfile):
# 	sim = Simulator('pic24')
# 	sim.set_param('model', arch['simulator_model'])
# 	sim.load(test_elf)
# 	sim.attach_log()
# 	sim.execute()

# 	try:
# 		sim.wait(5)
# 		sim.finish()
# 		logdata = sim.get_log()
# 	except TimeoutError:
# 		logdata = 'Test timed out (5 second timeout)'

# 	with open(logfile) as f:
# 		f.write(logdata)
