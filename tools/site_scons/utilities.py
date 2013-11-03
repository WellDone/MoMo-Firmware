#utilities.py

from SCons.Script import *
from SCons.Environment import Environment
import os
import fnmatch
import json as json

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

def load_settings():
	directory = os.path.dirname(__file__)
	filename = os.path.join(directory,'..','..','config','build_settings.json')

	with open(filename,'r') as f:
		return json.load(f)

	ValueError('Could not load global build settings file (config/build_settings.json)')

def build_app_for_chip(chip, define):
	"""
	Configure Scons to build an application module for the 8bit pic microchip indicated in the arguments.
	"""

	builddir = os.path.join('build', chip)
	VariantDir(builddir, 'src', duplicate=0)

	env = Environment(tools=['xc8_compiler', 'patch_mib12'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	env['CHIP'] = chip
	env['CHIPDEFINE'] = define
	
	#Load in all of the xc8 configuration from build_settings
	mib12conf = MIB12Config()
	mib12conf.config_env_for_app(env)

	Export('env')
	SConscript(os.path.join(builddir, 'SConscript'))

	return os.path.join(builddir, 'mib12_app_module.hex')

class MIB12Config:
	def __init__(self):
		conf = load_settings()

		self.rom_range = conf['mib12']['executive_rom_range']
		self.app_ram = conf['mib12']['application_ram']
		self.executive_ram = conf['mib12']['executive_ram']
		self.common_flags = conf['mib12']['common_xc8_flags']
		self.exec_flags = conf['mib12']['exec_xc8_flags']
		self.app_flags = conf['mib12']['app_xc8_flags']

	def config_env_for_exec(self, env):
		"""
		Configure the SCons build environment for building the MIB12 Executive
		"""

		self._ensure_flags(env)

		env['ROMSTART'] = self.rom_range[0]
		env['ROMEND'] = self.rom_range[1]

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.exec_flags


		env['RAMEXCLUDE'] = [self.app_ram]

	def config_env_for_app(self, env):
		"""
		Configure the SCons build environment for building the MIB12 Executive
		"""

		self._ensure_flags(env)

		env['ROMSTART'] = self.rom_range[1]+1

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.app_flags

		#MIB12 Executive takes all ram in first 
		env['RAMEXCLUDE'] = self.executive_ram

	def _ensure_flags(self, env):
		if "XC8FLAGS" not in env:
			env['XC8FLAGS'] = []