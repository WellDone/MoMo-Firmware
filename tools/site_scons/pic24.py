from SCons.Script import *
from SCons.Environment import Environment

def build_module(module_name, chip):
	"""
	Configure Scons to build a hex module for the pic24 chip listed in the argument.
	"""

	dirs = chip.build_dirs()
	output_name = '%s_%s.elf' % (module_name, chip.name)
	output_hex = '%s_%s.hex' % (module_name, chip.name)

	VariantDir(dirs['build'], 'src', duplicate=0)

	env = Environment(tools=['xc16_compiler', 'xc16_assembler', 'xc16_linker'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	env['CHIP'] = chip
	env['OUTPUT'] = output_name
	Export('env')
	
	SConscript(os.path.join(dirs['build'], 'SConscript'))

	elffile = os.path.join(dirs['build'], output_name)
	hexfile = os.path.join(dirs['build'], output_hex)

	env.Command(hexfile, elffile, 'xc16-bin2hex %s' % elffile)
	
	output = env.InstallAs(os.path.join(dirs['output'], output_hex), hexfile)
	return [output]