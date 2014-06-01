#xc16_linker.py
#SCons Builder Actions for the XC16 PIC assembler

import SCons.Builder
import os.path
import sys
import utilities

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))
from pymomo.utilities.paths import convert_path

def xc16_generator(source, target, env, for_signature):
	"""
	Create an XC16 command line using the parameter defined in 
	the environment
	"""

	chip = env['CHIP']

	#Build up the command line
	args = ['xc16-gcc']
	args.extend(['-mcpu=%s' % chip.name.upper()])
	args.extend([str(x) for x in source])
	args.extend(['-o %s' % (str(target[0]))])
	args.extend(utilities.build_libdirs(chip.property('libdirs', default=[])))
	args.extend(utilities.build_staticlibs(chip.property('libraries', default=[]), chip))
	args.extend(chip.property('ldflags', default=[]))

	linker_script = chip.property('linker', default=None)
	if linker_script is not None:
		args.append('-T%s' % linker_script)

	return " ".join(args)

_xc16_obj = SCons.Builder.Builder(
	generator = xc16_generator,
	suffix='.o')

def generate(env):
	env['BUILDERS']['xc16_ld'] = _xc16_obj

def exists(env):
	return 1