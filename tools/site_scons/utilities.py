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

def build_app_for_chip(chip):
	"""
	Configure Scons to build an application module for the 8bit pic microchip indicated in the argument chip.
	"""

	builddir = os.path.join('build', chip)
	VariantDir(builddir, 'src', duplicate=0)

	env = Environment(tools=['xc8_compiler', 'patch_mib12'], ENV = os.environ)
	env.AppendENVPath('PATH','../../tools/scripts')
	
	#Load in all of the xc8 configuration from build_settings
	mib12conf = MIB12Config()
	mib12conf.config_env_for_app(env)
	mib12conf.config_env_for_chip(chip, env)
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
		self.common_incs = conf['mib12']['includes']

		self.common_incs = map(lambda x: os.path.normpath(x), self.common_incs); #Change path separator on Windows if necessary

		#Load chip information
		chips = conf['mib12']['chip_aliases']

		self.chip_names = {}

		for key,aliases in chips.iteritems():
			self.chip_names[key] = set(aliases)

		self.chip_info = {}
		chip_defs = conf['mib12']['chip_definitions']
		
		for key in chips.iterkeys():
			self.chip_info[key] = chip_defs[key]

	def add_common_incs(self, env):
		"""
		Add common include directories for 8 bit pics from build_settings.json
		"""
		if 'INCLUDE' not in env:
			env['INCLUDE'] = []

		env['INCLUDE'] += self.common_incs

	def config_env_for_exec(self, env, chip):
		"""
		Configure the SCons build environment for building the MIB12 Executive
		"""

		self._ensure_flags(env)

		env['ROMSTART'] = self.rom_range[0]
		env['ROMEND'] = self.rom_range[1]

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.exec_flags

		self.config_env_for_chip(chip, env)
		self.add_common_incs(env)

		env['RAMEXCLUDE'] = [self.app_ram]

	def config_env_for_app(self, env, chip):
		"""
		Configure the SCons build environment for building a MIB12 application module
		"""

		self._ensure_flags(env)

		env['ROMSTART'] = self.rom_range[1]+1

		env['XC8FLAGS'] += self.common_flags
		env['XC8FLAGS'] += self.app_flags

		self.config_env_for_chip(chip, env)
		self.add_common_incs(env)

		#MIB12 Executive takes all ram in first 
		env['RAMEXCLUDE'] = self.executive_ram

	def config_env_for_chip(self, chip, env):
		"""
		Configure SCons environment to build for a specific 8-bit chip
		"""

		try:
			chip,define,sim = self.find_chip_info(chip)

			env['CHIP'] = chip
			env['CHIPDEFINE'] = define
		except KeyError:
			raise ValueError("Chip %s not found in build_settings.json, cannot target that chip." % chip)

	def find_chip_info(self, chip):
		found = None
		for key,vals in self.chip_names.iteritems():
			if chip in vals:
				found = key
				break

		if found is None:
			raise KeyError("Chip not found")

		return self.chip_info[found]


	def _ensure_flags(self, env):
		if "XC8FLAGS" not in env:
			env['XC8FLAGS'] = []