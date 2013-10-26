#xc8_compiler.py
#SCons Builder Actions for the XC8 PIC compiler

import SCons.Builder
import os.path

xc8_flags = []
xc8_flags.append('-G')
xc8_flags.append('--double=24')
xc8_flags.append('--float=24')
xc8_flags.append('--addrqual=require')
xc8_flags.append('')

args = " ".join(xc8_flags)


def xc8_generator(source, target, env, for_signature):
	"""
	Create an XC8 command line
	"""

	incs = []

	if 'INCLUDE' in env:
		incs = map(lambda x: '-I'+str(x), env['INCLUDE'])

	src = " ".join(map(lambda x: str(x), source))
	out = '-o' + str(target[0])

	inc_statement = " ".join(incs)

	return 'xc8 ' + \
			make_chip(env) + " " + env['XC8FLAGS'] + " " + \
			inc_statement + " " + args + src + " " + out

def xc8_emit_int_files(target, source, env):
	intfiles = map(lambda x: make_int_files(x), source)

	intfiles = reduce(lambda x,y: x+y, intfiles, [])

	target += intfiles + make_int_target_files(target[0])
	target += make_int_files('startup.as')
	target += ['startup.as']

	return target, source

def make_chip(env):
	return '--chip=%s -D%s' % (env['CHIP'], env['CHIPDEFINE'])

def make_int_files(cfile):
	"""
	Given an Scons node representing a c or as file, create the xc8 intermediate targets
	"""
	name,ext = os.path.splitext(os.path.basename(str(cfile)))

	if ext == ".c":
		return [name + '.d', name + '.p1', name + '.pre']
	elif ext == ".as":
		return [name + '.d', name + '.lst', name + '.pre', name + '.obj', name + '.rlf']

	return []

def make_int_target_files(target):
	name,ext = os.path.splitext(os.path.basename(str(target)))

	return [name+'.as', name+'.cmf',name+'.cof',name+'.hxl',name+'.sdb',name+'.sym', name + '.lst', name + '.obj', name + '.rlf']


_xc8_hex = SCons.Builder.Builder(
	generator = xc8_generator,
	emitter = xc8_emit_int_files)

def generate(env):
	env['BUILDERS']['xc8'] = _xc8_hex

def exists(env):
	return 1