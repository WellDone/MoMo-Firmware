#utilities.py

from SCons.Script import *
from SCons.Environment import Environment
import os
import fnmatch
import json as json
import sys
import os.path
import pic12

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from pymomo.utilities import build
from pymomo.mib.config12 import MIB12Processor

def find_files(dirname, pattern):
	"""
	Recursively find all files matching pattern under path dirname
	"""

	matches = []
	for root, dirnames, filenames in os.walk(dirname, followlinks=True):
		print dirnames, filenames
		for filename in fnmatch.filter(filenames, pattern):
			matches.append(os.path.join(root,filename))

	return matches

def build_includes(includes):
	if isinstance(includes, basestring):
		includes = [includes]

	return ['-I"%s"' % x for x in includes]

def build_libdirs(libdirs):
	if isinstance(libdirs, basestring):
		libdirs = [libdirs]

	return ['-L"%s"' % x for x in libdirs]

def build_staticlibs(libs, chip):
	if isinstance(libs, basestring):
		libs = [libs]

	#Append chip type and suffix
	libs = ["%s_%s" % (x, chip.arch_name()) for x in libs]

	return ['-l%s' % x for x in libs]

def build_defines(defines):
	return ['-D%s=%s' % (x,str(y)) for x,y in defines.iteritems()]

def get_family(fam):
	return build.ChipFamily(fam)