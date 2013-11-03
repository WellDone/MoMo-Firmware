import SCons.Builder
import scansym

def build_symbols(target, source, env):
	print source
	symtab = scansym.XC8SymbolTable(str(source[0]))
	symtab.generate_h_file(str(target[0]))

_build_sym_h = SCons.Builder.Builder(
	action = build_symbols
	)

def generate(env):
	env['BUILDERS']['build_symbols'] = _build_sym_h

def exists(env):
	return 1

