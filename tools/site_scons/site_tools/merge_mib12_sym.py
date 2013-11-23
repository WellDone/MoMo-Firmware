import SCons.Builder
import scansym

def merge_sym(target, source, env):
	"""
	Given two source files, the first being the mib12_executive and the second
	being an application hex, merge the two into a complete application hex
	"""

	first = scansym.XC8SymbolTable(str(source[0]))

	first.merge(str(source[1]))
	first.generate_stb_file(str(target[0]))

_merge_sym = SCons.Builder.Builder(
	action = merge_sym
	)

def generate(env):
	env['BUILDERS']['merge_mib12_symbols'] = _merge_sym

def exists(env):
	return 1

