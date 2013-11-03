#patch_mib12.py

import SCons.Builder

_patch_mib12 = SCons.Builder.Builder(
	action = 'patch_mib12_api.py $SOURCES $TARGET'
	)

def generate(env):
	env['BUILDERS']['patch_mib12'] = _patch_mib12

def exists(env):
	return 1